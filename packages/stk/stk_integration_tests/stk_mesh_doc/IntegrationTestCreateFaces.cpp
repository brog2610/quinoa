// Copyright (c) 2013, Sandia Corporation.
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//
//     * Neither the name of Sandia Corporation nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <stddef.h>                     // for size_t
#include <stk_util/parallel/Parallel.hpp>  // for ParallelMachine, etc
#include <stk_mesh/base/BulkData.hpp>   // for BulkData
#include <stk_mesh/base/Comm.hpp>       // for comm_mesh_counts
#include <stk_mesh/base/CreateFaces.hpp>  // for create_faces
#include <stk_mesh/base/MetaData.hpp>   // for MetaData, put_field
#include <stk_mesh/base/SkinMesh.hpp>   // for skin_mesh
#include <stk_mesh/fixtures/GearsFixture.hpp>  // for GearsFixture, etc
#include <stk_mesh/fixtures/TetFixture.hpp>  // for TetFixture
#include <stk_mesh/fixtures/degenerate_mesh.hpp>  // for VectorFieldType, etc
#include <stk_mesh/fixtures/heterogeneous_mesh.hpp>
#include <gtest/gtest.h>
#include <vector>                       // for vector, vector<>::iterator
#include "stk_mesh/base/Bucket.hpp"     // for Bucket
#include "stk_mesh/base/Entity.hpp"     // for Entity
#include "stk_mesh/base/Part.hpp"       // for Part
#include "stk_mesh/base/Selector.hpp"   // for Selector, operator&, etc
#include "stk_mesh/base/Types.hpp"      // for BucketVector, EntityRank
#include "stk_topology/topology.hpp"    // for topology, etc
#include "stk_io/StkMeshIoBroker.hpp"
#include <stk_mesh/base/GetEntities.hpp>
#include <stk_unit_test_utils/FaceTestingUtils.hpp>

namespace
{


void read_file_dump_mesh(std::string filename, bool create_faces) {
    stk::io::StkMeshIoBroker stkMeshIoBroker(MPI_COMM_WORLD);
    stkMeshIoBroker.set_sideset_face_creation_behavior(stk::io::StkMeshIoBroker::STK_IO_SIDESET_FACE_CREATION_CURRENT);
    stkMeshIoBroker.add_mesh_database(filename, stk::io::READ_MESH);
    stkMeshIoBroker.create_input_mesh();
    stkMeshIoBroker.populate_bulk_data();
    stk::mesh::BulkData &mesh = stkMeshIoBroker.bulk_data();
    std::cout << "filename: " << filename << std::endl;
    if (create_faces) {
        stk::mesh::create_faces(mesh);
    }
    mesh.dump_all_mesh_info(std::cout);

}


//The Magical Alphabet of Hexes, Shells & Sidesets
//
// A = hex in block A
// B = hex in block B
// e = shell in block E
// f = shell in block F
// L = sideset associated with the side on the left
// R = "        "           "   "  "     "   " right
// D = sideset containing 2 sides, one associated to left and one to right
// X = sideset associated with all sides on this surface
// J = two hexes in block A connected to the same 8 nodes
//
// .e = the language of our Patron Saint Exodus
//
// RR = pronounced like a pirate
// RRR = roll the R


TEST(StkIo, confirm_face_connectivity_AeA)
{
    MPI_Comm communicator = MPI_COMM_WORLD;
    if(stk::parallel_machine_size(communicator) != 1) { return; }

    stk::io::StkMeshIoBroker stkMeshIoBroker(communicator);
    stkMeshIoBroker.set_sideset_face_creation_behavior(stk::io::StkMeshIoBroker::STK_IO_SIDESET_FACE_CREATION_CURRENT);
    stkMeshIoBroker.add_mesh_database("AeA.e", stk::io::READ_MESH);
    stkMeshIoBroker.create_input_mesh();
    stkMeshIoBroker.populate_bulk_data();
    stk::mesh::BulkData &mesh = stkMeshIoBroker.bulk_data();

    stk::mesh::create_faces(mesh);

    //want to specifically check this connectivity
    //
    //  -----   F   S   F   -----
    //  |   |   A   H   A   |   |
    //  |HEX|-5>C<0-E-1>C<4-|HEX|
    //  | A |   E   L   E   | B |
    //  -----   D   L   C   -----
    //
    // -0> means the 0th ordinal face connection, -1> means the 1st, etc.
    //
    // since this is the current behavior and want to test it in STK as well

    bool face1okay = false;
    bool face2okay = false;
    typedef std::vector<stk::mesh::EntityId>  EntityIdVector;
    stk::mesh::BucketVector const & face_buckets = mesh.buckets(stk::topology::FACE_RANK);
    for (size_t bucket_count=0, bucket_end=face_buckets.size(); bucket_count < bucket_end; ++bucket_count) {
        stk::mesh::Bucket & bucket = *face_buckets[bucket_count];
        for (size_t face_count=0, face_end=bucket.size(); face_count < face_end; ++face_count) {
            stk::mesh::Entity face = bucket[face_count];
            stk::mesh::EntityKey face_key = mesh.entity_key(face);
            stk::mesh::Entity const * elements = mesh.begin_elements(face);
            for (unsigned elem_count = 0; elem_count < mesh.num_elements(face); ++elem_count) {
                for (unsigned other_elem_count = elem_count;
                        other_elem_count < mesh.num_elements(face); ++other_elem_count) {
                    if ((elem_count != other_elem_count) &&
                            (elements[elem_count] != elements[other_elem_count])) {
                        stk::mesh::Entity elem1 = elements[elem_count];
                        stk::mesh::Entity const * face_array1 = mesh.begin_faces(elem1);
                        stk::mesh::Entity elem2 = elements[other_elem_count];
                        stk::mesh::Entity const * face_array2 = mesh.begin_faces(elem2);
                        if (mesh.bucket(elem1).topology() == stk::topology::HEX_8 && mesh.entity_key(face_array1[5]) == face_key &&
                                mesh.bucket(elem2).topology() == stk::topology::SHELL_QUAD_4 && mesh.entity_key(face_array2[0]) == face_key) {
                            face1okay = true;

                            stk::topology faceTopology = mesh.bucket(face).topology();
                            EntityIdVector face_on_shell_node_ids(faceTopology.num_nodes());
                            EntityIdVector face_on_hex_node_ids(faceTopology.num_nodes());

                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_hex_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array1[5])[count]).id();
                            }
                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_shell_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array2[0])[count]).id();
                            }

                            unsigned permutation = faceTopology.lexicographical_smallest_permutation_preserve_polarity(face_on_shell_node_ids, face_on_hex_node_ids);
                            EXPECT_LT(permutation, faceTopology.num_positive_permutations());
                        }
                        if (mesh.bucket(elem1).topology() == stk::topology::SHELL_QUAD_4 &&  mesh.entity_key(face_array1[0]) == face_key &&
                                mesh.bucket(elem2).topology() == stk::topology::HEX_8 &&  mesh.entity_key(face_array2[5]) == face_key) {
                            face1okay = true;
                            stk::topology faceTopology = mesh.bucket(face).topology();
                            EntityIdVector face_on_shell_node_ids(faceTopology.num_nodes());
                            EntityIdVector face_on_hex_node_ids(faceTopology.num_nodes());

                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_hex_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array2[5])[count]).id();
                            }
                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_shell_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array1[0])[count]).id();
                            }

                            unsigned permutation = faceTopology.lexicographical_smallest_permutation_preserve_polarity(face_on_shell_node_ids, face_on_hex_node_ids);
                            EXPECT_LT(permutation, faceTopology.num_positive_permutations());
                        }
                        if (mesh.bucket(elem1).topology() == stk::topology::HEX_8 &&  mesh.entity_key(face_array1[4]) == face_key &&
                                mesh.bucket(elem2).topology() == stk::topology::SHELL_QUAD_4 &&  mesh.entity_key(face_array2[1]) == face_key) {
                            face2okay = true;
                            stk::topology faceTopology = mesh.bucket(face).topology();
                            EntityIdVector face_on_shell_node_ids(faceTopology.num_nodes());
                            EntityIdVector face_on_hex_node_ids(faceTopology.num_nodes());

                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_hex_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array1[4])[count]).id();
                            }
                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_shell_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array2[1])[count]).id();
                            }

                            unsigned permutation = faceTopology.lexicographical_smallest_permutation_preserve_polarity(face_on_shell_node_ids, face_on_hex_node_ids);
                            EXPECT_LT(permutation, faceTopology.num_positive_permutations());
                        }
                        if (mesh.bucket(elem1).topology() == stk::topology::SHELL_QUAD_4 &&  mesh.entity_key(face_array1[1]) == face_key &&
                                mesh.bucket(elem2).topology() == stk::topology::HEX_8 &&  mesh.entity_key(face_array2[4]) == face_key) {
                            face2okay = true;

                            stk::topology faceTopology = mesh.bucket(face).topology();
                            EntityIdVector face_on_shell_node_ids(faceTopology.num_nodes());
                            EntityIdVector face_on_hex_node_ids(faceTopology.num_nodes());

                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_hex_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array2[4])[count]).id();
                            }
                            for (unsigned count=0; count < faceTopology.num_nodes(); ++count) {
                                face_on_shell_node_ids[count] = mesh.entity_key(mesh.begin_nodes(face_array1[1])[count]).id();
                            }

                            unsigned permutation = faceTopology.lexicographical_smallest_permutation_preserve_polarity(face_on_shell_node_ids, face_on_hex_node_ids);
                            EXPECT_LT(permutation, faceTopology.num_positive_permutations());

                        }


                    }
                }
            }
        }
    }
    EXPECT_TRUE(face1okay);
    EXPECT_TRUE(face2okay);
}

TEST(StkIo, DISABLED_dump_mesh)
{
//    read_file_dump_mesh("ALeDfRA.e", true);
//    read_file_dump_mesh("ALeXfRA.e", true);
    read_file_dump_mesh("ADe.e", false);
}

} // empty namespace
