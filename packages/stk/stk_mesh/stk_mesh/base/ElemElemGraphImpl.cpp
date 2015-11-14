#include "ElemElemGraphImpl.hpp"
#include "ElemElemGraph.hpp"

#include <vector>
#include <algorithm>

#include <stk_topology/topology.hpp>
#include <stk_mesh/base/BulkData.hpp>
#include <stk_mesh/base/MetaData.hpp>
#include <stk_mesh/base/Comm.hpp>
#include <stk_mesh/base/FEMHelpers.hpp>
#include <stk_mesh/base/GetEntities.hpp>
#include <stk_mesh/baseImpl/MeshImplUtils.hpp>

#include <stk_util/parallel/CommSparse.hpp>
#include <stk_util/parallel/ParallelReduce.hpp>
#include <stk_util/environment/ReportHandler.hpp>

namespace stk { namespace mesh {

namespace impl
{

void set_local_ids_and_fill_element_entities_and_topologies(stk::mesh::BulkData& bulkData, stk::mesh::EntityVector& local_id_to_element_entity, std::vector<stk::topology>& element_topologies)
{
    const stk::mesh::BucketVector& elemBuckets = bulkData.get_buckets(stk::topology::ELEM_RANK, bulkData.mesh_meta_data().locally_owned_part());
    size_t local_id = 0;
    for(size_t i=0; i<elemBuckets.size(); ++i)
    {
        const stk::mesh::Bucket& bucket = *elemBuckets[i];
        for(size_t j=0; j<bucket.size(); ++j)
        {
            local_id_to_element_entity[local_id] = bucket[j];
            element_topologies[local_id] = bucket.topology();
            bulkData.set_local_id(bucket[j], local_id);
            local_id++;
        }
    }
}

void fill_local_ids_and_fill_element_entities_and_topologies(stk::mesh::BulkData& bulkData, stk::mesh::EntityVector& local_id_to_element_entity,
                                                             std::vector<impl::LocalId>& entity_to_local_id, std::vector<stk::topology>& element_topologies)
{
    const stk::mesh::BucketVector & elemBuckets = bulkData.get_buckets(stk::topology::ELEM_RANK, bulkData.mesh_meta_data().locally_owned_part());
    size_t local_id = 0;
    for(size_t i=0; i<elemBuckets.size(); ++i)
    {
        const stk::mesh::Bucket& bucket = *elemBuckets[i];
        for(size_t j=0; j<bucket.size(); ++j)
        {
            local_id_to_element_entity[local_id] = bucket[j];
            element_topologies[local_id] = bucket.topology();
            entity_to_local_id[bucket[j].local_offset()] = local_id;
            local_id++;
        }
    }
}

ElemSideToProcAndFaceId get_element_side_ids_to_communicate(const stk::mesh::BulkData& bulkData)
{
    stk::mesh::EntityVector elements_to_communicate;
    std::set<stk::mesh::Entity> element_set;
    const stk::mesh::BucketVector& shared_node_buckets = bulkData.get_buckets(stk::topology::NODE_RANK, bulkData.mesh_meta_data().globally_shared_part());
    for(size_t i=0; i<shared_node_buckets.size(); ++i)
    {
        const stk::mesh::Bucket& bucket = *shared_node_buckets[i];
        for(size_t node_index=0; node_index<bucket.size(); ++node_index)
        {
            stk::mesh::Entity node = bucket[node_index];
            const stk::mesh::Entity* elements = bulkData.begin_elements(node);
            unsigned num_elements = bulkData.num_elements(node);
            for(unsigned element_index=0; element_index<num_elements; ++element_index)
            {
                if (bulkData.bucket(elements[element_index]).owned())
                {
                    element_set.insert(elements[element_index]);
                }
            }
        }
    }
    elements_to_communicate.assign(element_set.begin(), element_set.end());

    return build_element_side_ids_to_proc_map(bulkData, elements_to_communicate);
}


ElemSideToProcAndFaceId get_element_side_ids_to_communicate(const stk::mesh::BulkData& bulkData, const stk::mesh::EntityVector &element_list)
{
    stk::mesh::EntityVector elements_to_communicate;

    for(const stk::mesh::Entity &element : element_list )
    {
        const stk::mesh::Entity* nodes = bulkData.begin_nodes(element);
        int numNodes = bulkData.num_nodes(element);

        for(int i=0; i<numNodes; ++i)
        {
            stk::mesh::Entity node = nodes[i];

            if(bulkData.bucket(node).shared())
            {
                elements_to_communicate.push_back(element);
                break;
            }
        }
    }

    return build_element_side_ids_to_proc_map(bulkData, elements_to_communicate);
}


ElemSideToProcAndFaceId build_element_side_ids_to_proc_map(const stk::mesh::BulkData& bulkData, const stk::mesh::EntityVector &elements_to_communicate)
{
    ElemSideToProcAndFaceId elem_side_comm;

    for(size_t i=0;i<elements_to_communicate.size();++i)
    {
        stk::mesh::Entity elem = elements_to_communicate[i];
        const stk::mesh::Entity* elem_nodes = bulkData.begin_nodes(elem);
        stk::topology elem_top = bulkData.bucket(elem).topology();
        unsigned num_sides = elem_top.num_sides();
        for(unsigned side=0;side<num_sides;++side)
        {
            stk::topology side_top = elem_top.side_topology(side);
            unsigned num_nodes_this_side = side_top.num_nodes();
            stk::mesh::EntityVector side_nodes(num_nodes_this_side);
            elem_top.side_nodes(elem_nodes, side, side_nodes.begin());
            std::vector<stk::mesh::EntityKey> keys(num_nodes_this_side);
            for(size_t j=0;j<keys.size();++j)
            {
                keys[j] = bulkData.entity_key(side_nodes[j]);
            }
            std::vector<int> sharing_procs;
            bulkData.shared_procs_intersection(keys, sharing_procs);
            for (int proc: sharing_procs) {
                elem_side_comm.insert(std::pair<EntitySidePair, ProcFaceIdPair>(EntitySidePair(elem, side), ProcFaceIdPair(proc,0)));
            }
        }
    }
    return elem_side_comm;
}

size_t pack_shared_side_nodes_of_elements(stk::CommSparse& comm,
                                        const stk::mesh::BulkData& bulkData,
                                        ElemSideToProcAndFaceId &elements_to_communicate,
                                        const std::vector<stk::mesh::EntityId>& suggested_side_ids,
                                        const stk::mesh::Part &part)
{
    ElemSideToProcAndFaceId::iterator iter = elements_to_communicate.begin();
    ElemSideToProcAndFaceId::const_iterator end = elements_to_communicate.end();
    size_t counter = 0;

    for(; iter!= end; ++iter)
    {
        stk::mesh::Entity elem = iter->first.entity;
        unsigned side_index    = iter->first.side_id;
        int sharing_proc       = iter->second.proc;
        LocalId element_id     = bulkData.identifier(elem);
        stk::mesh::EntityId suggested_side_id = suggested_side_ids[counter];
        ++counter;
        iter->second.side_id = suggested_side_id;

        stk::topology topology = bulkData.bucket(elem).topology();
        const bool isInPart = bulkData.bucket(elem).member(part);
        const stk::mesh::Entity* elem_nodes = bulkData.begin_nodes(elem);

        unsigned num_nodes_this_side = topology.side_topology(side_index).num_nodes();
        stk::mesh::EntityVector side_nodes(num_nodes_this_side);
        topology.side_nodes(elem_nodes, side_index, side_nodes.begin());

        std::vector<stk::mesh::EntityKey> side_node_entity_keys(num_nodes_this_side);
        for(size_t i=0; i<num_nodes_this_side; ++i)
        {
            side_node_entity_keys[i] = bulkData.entity_key(side_nodes[i]);
        }

        comm.send_buffer(sharing_proc).pack<LocalId>(element_id);
        comm.send_buffer(sharing_proc).pack<stk::topology>(topology);
        comm.send_buffer(sharing_proc).pack<unsigned>(side_index);
        comm.send_buffer(sharing_proc).pack<stk::mesh::EntityId>(suggested_side_id);
        comm.send_buffer(sharing_proc).pack<bool>(isInPart);
        comm.send_buffer(sharing_proc).pack<unsigned>(num_nodes_this_side);
        for(size_t i=0; i<num_nodes_this_side; ++i)
        {
            comm.send_buffer(sharing_proc).pack<stk::mesh::EntityKey>(side_node_entity_keys[i]);
        }
    }
    return counter;
}


bool does_element_have_side(stk::mesh::BulkData& bulkData, stk::mesh::Entity element)
{
    unsigned dimension_of_element = bulkData.bucket(element).topology().dimension();
    unsigned dimension_of_mesh = bulkData.mesh_meta_data().spatial_dimension();
    return dimension_of_element == dimension_of_mesh;
}

std::vector<graphEdgeProc> get_elements_to_communicate(stk::mesh::BulkData& bulkData, const stk::mesh::EntityVector &killedElements,
        const ElemElemGraph& elem_graph)
{
    std::vector<graphEdgeProc> elements_to_comm;

    for(size_t i=0;i<killedElements.size();++i)
    {
        stk::mesh::Entity this_elem_entity = killedElements[i];
        for(size_t j=0;j<elem_graph.get_num_connected_elems(this_elem_entity);++j)
        {
            if(does_element_have_side(bulkData, this_elem_entity) && !elem_graph.is_connected_elem_locally_owned(this_elem_entity, j))
            {
                stk::mesh::EntityId other_element_id = elem_graph.get_entity_id_of_remote_element(this_elem_entity,j);
                int other_proc = elem_graph.get_owning_proc_id_of_remote_element(this_elem_entity, other_element_id);
                elements_to_comm.push_back(graphEdgeProc(bulkData.identifier(this_elem_entity), other_element_id, other_proc));
            }
        }
    }

    return elements_to_comm;
}

void pack_elements_to_comm(stk::CommSparse &comm, const std::vector<graphEdgeProc>& elements_to_comm)
{
    for(size_t i=0;i<elements_to_comm.size();++i)
    {
        int remote_proc = elements_to_comm[i].m_proc_id;
        stk::mesh::EntityId localId = elements_to_comm[i].m_localElementId;
        stk::mesh::EntityId remoteId = elements_to_comm[i].m_remoteElementId;

        comm.send_buffer(remote_proc).pack<stk::mesh::EntityId>(localId);
        comm.send_buffer(remote_proc).pack<stk::mesh::EntityId>(remoteId);
    }
}

void communicate_killed_entities(stk::mesh::BulkData& bulkData, const std::vector<graphEdgeProc>& elements_to_comm,
        std::vector<std::pair<stk::mesh::EntityId, stk::mesh::EntityId> >& remote_edges)
{
    stk::CommSparse comm(bulkData.parallel());
    pack_elements_to_comm(comm, elements_to_comm);
    comm.allocate_buffers();
    pack_elements_to_comm(comm, elements_to_comm);
    comm.communicate();

    for(int i=0;i<bulkData.parallel_size();++i)
    {
        while(comm.recv_buffer(i).remaining())
        {
            stk::mesh::EntityId remoteId;
            stk::mesh::EntityId localId;
            comm.recv_buffer(i).unpack<stk::mesh::EntityId>(remoteId);
            comm.recv_buffer(i).unpack<stk::mesh::EntityId>(localId);
            remote_edges.push_back(std::make_pair(localId, remoteId));
        }
    }
}

int get_element_side_multiplier()
{
    return 1000;
}

stk::mesh::EntityId get_side_id_for_remotely_connected_element(stk::mesh::EntityId local_element_id,
        stk::mesh::EntityId local_side_id, stk::mesh::EntityId remote_element_id,
        stk::mesh::EntityId remote_side_id, size_t& id_counter)
{
    stk::mesh::EntityId side_global_id = 0;
    if(local_element_id < remote_element_id)
    {
        side_global_id = local_side_id;
        id_counter++;
    }
    else
    {
        side_global_id = remote_side_id;
    }

    return side_global_id;
}

stk::mesh::Permutation get_permutation_for_new_side(const parallel_info& parallel_edge_info, stk::mesh::EntityId local_element_id, stk::mesh::EntityId remote_element_id)
{
    stk::mesh::Permutation perm;
    if(local_element_id < remote_element_id)
    {
        perm = static_cast<stk::mesh::Permutation>(0);
    }
    else
    {
        perm = static_cast<stk::mesh::Permutation>(parallel_edge_info.m_permutation);
    }
    return perm;
}

bool is_id_already_in_use_locally(stk::mesh::BulkData& bulkData, stk::mesh::EntityRank rank, stk::mesh::EntityId id)
{
    stk::mesh::Entity entity = bulkData.get_entity(rank, id);
    return bulkData.is_valid(entity);
}

bool does_side_exist_with_different_permutation(stk::mesh::BulkData& bulkData, stk::mesh::Entity element,
        stk::mesh::ConnectivityOrdinal side_ordinal, stk::mesh::Permutation perm)
{
    stk::mesh::EntityRank side_rank = bulkData.mesh_meta_data().side_rank();

    unsigned elem_num_sides = bulkData.num_connectivity(element, side_rank);
    const stk::mesh::ConnectivityOrdinal * elem_ord_it = bulkData.begin_ordinals(element, side_rank);
    const stk::mesh::Permutation * elem_perm_it = bulkData.begin_permutations(element, side_rank);

    for (unsigned i=0 ; i<elem_num_sides ; ++i)
    {
        if (elem_ord_it[i] == static_cast<unsigned>(side_ordinal))
        {
            if (perm != elem_perm_it[i])
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

bool does_element_side_exist(stk::mesh::BulkData& bulkData, stk::mesh::Entity element, stk::mesh::ConnectivityOrdinal side_ordinal)
{
    stk::mesh::Entity side = stk::mesh::Entity();
    stk::mesh::EntityRank side_rank = bulkData.mesh_meta_data().side_rank();

    unsigned elem_num_sides = bulkData.num_connectivity(element, side_rank);
    const stk::mesh::Entity * elem_sides = bulkData.begin(element, side_rank);
    const stk::mesh::ConnectivityOrdinal * elem_ord_it = bulkData.begin_ordinals(element, side_rank);
    for (unsigned i=0 ; i<elem_num_sides ; ++i)
    {
        if (elem_ord_it[i] == static_cast<unsigned>(side_ordinal))
        {
            side = elem_sides[i];
            break;
        }
    }

    return bulkData.is_valid(side);
}

stk::mesh::Entity get_side_for_element(const stk::mesh::BulkData& bulkData, stk::mesh::Entity element, int side_id)
{
    stk::mesh::EntityRank side_rank = bulkData.mesh_meta_data().side_rank();

    unsigned num_sides = bulkData.num_connectivity(element, side_rank);
    const stk::mesh::Entity *sides = bulkData.begin(element, side_rank);
    const stk::mesh::ConnectivityOrdinal *ordinals = bulkData.begin_ordinals(element, side_rank);

    stk::mesh::Entity side;

    for(unsigned ii = 0; ii < num_sides; ++ii)
    {
        if(ordinals[ii] == static_cast<stk::mesh::ConnectivityOrdinal>(side_id))
        {
            side = sides[ii];
            break;
        }
    }
    return side;
}


bool create_or_delete_shared_side(stk::mesh::BulkData& bulkData, const parallel_info& parallel_edge_info, const ElemElemGraph& elementGraph,
        stk::mesh::Entity local_element, stk::mesh::EntityId remote_id, bool create_shared_side, const stk::mesh::PartVector& side_parts,
        stk::mesh::Part &activePart, std::vector<stk::mesh::sharing_info> &shared_modified, stk::mesh::EntityVector &deletedEntities,
        stk::mesh::EntityVector & facesWithNodesToBeMarkedInactive, stk::mesh::Part& sides_created_during_death)
{
    bool topology_modified = false;

    int side_id = elementGraph.get_side_from_element1_to_remote_element2(local_element, remote_id);
    ThrowRequireMsg(side_id != -1, "Program error. Please contact sierra-help@sandia.gov for support.");

    stk::mesh::EntityId side_global_id = parallel_edge_info.m_chosen_side_id;
    stk::mesh::ConnectivityOrdinal side_ord = static_cast<stk::mesh::ConnectivityOrdinal>(side_id);
    std::string msg = "Program error. Contact sierra-help@sandia.gov for support.";

    stk::topology side_top = bulkData.bucket(local_element).topology().side_topology(side_ord);
    if(create_shared_side)
    {
        // determine which element is active
        stk::mesh::Permutation perm = stk::mesh::DEFAULT_PERMUTATION;
        int owning_proc = bulkData.parallel_rank();
        int other_proc = parallel_edge_info.m_other_proc;

        if(parallel_edge_info.m_in_part)
        {
            perm = static_cast<stk::mesh::Permutation>(parallel_edge_info.m_permutation);
            owning_proc = other_proc;
        }

        stk::mesh::PartVector parts = side_parts;
        parts.push_back(&bulkData.mesh_meta_data().get_topology_root_part(side_top));

        stk::mesh::Entity side = stk::mesh::impl::get_side_for_element(bulkData, local_element, side_id);

        if(!bulkData.is_valid(side))
        {
            topology_modified = true;
            parts.push_back(&sides_created_during_death);
            ThrowRequireMsg(!impl::is_id_already_in_use_locally(bulkData, bulkData.mesh_meta_data().side_rank(), side_global_id), msg);
            side = connect_side_to_element(bulkData, local_element, side_global_id, side_ord, perm, parts);
            shared_modified.push_back(stk::mesh::sharing_info(side, other_proc, owning_proc));
        }
        else
        {
            topology_modified = true;
            if(bulkData.bucket(side).owned())
            {
                bulkData.change_entity_parts(side, parts, stk::mesh::PartVector());
            }
        }
    }
    else
    {
        stk::mesh::Entity side = stk::mesh::impl::get_side_for_element(bulkData, local_element, side_id);
        facesWithNodesToBeMarkedInactive.push_back(side);
        if(bulkData.is_valid(side) && bulkData.bucket(side).member(sides_created_during_death))
        {
            deletedEntities.push_back(side);
            topology_modified = true;
        }
        else if(bulkData.is_valid(side) && bulkData.bucket(side).owned())
        {
            topology_modified = true;
            bulkData.change_entity_parts(side, {}, {&activePart});
        }
    }
    return topology_modified;
}

stk::mesh::Entity connect_side_to_element(stk::mesh::BulkData& bulkData, stk::mesh::Entity element,
        stk::mesh::EntityId side_global_id, stk::mesh::ConnectivityOrdinal side_ordinal,
        stk::mesh::Permutation side_permutation, const stk::mesh::PartVector& parts)
{
    stk::mesh::EntityRank side_rank = bulkData.mesh_meta_data().side_rank();

    stk::mesh::Entity side = bulkData.declare_entity(side_rank, side_global_id, parts);

    // connect element to side
    bulkData.declare_relation(element, side, side_ordinal, side_permutation);

    // connect side to nodes
    const stk::mesh::Entity* nodes = bulkData.begin_nodes(element);
    stk::topology side_top = bulkData.bucket(element).topology().side_topology(side_ordinal);
    stk::mesh::EntityVector side_nodes(side_top.num_nodes());
    bulkData.bucket(element).topology().side_nodes(nodes, side_ordinal, side_nodes.begin());
    stk::mesh::EntityVector permuted_side_nodes(side_top.num_nodes());
    side_top.permutation_nodes(side_nodes, side_permutation, permuted_side_nodes.begin());
    for(size_t i=0;i<permuted_side_nodes.size();++i)
    {
        bulkData.declare_relation(side, permuted_side_nodes[i], i);
    }

    return side;
}

stk::mesh::EntityId get_side_global_id(const stk::mesh::BulkData &bulkData, const ElemElemGraph& elementGraph, stk::mesh::Entity element1, stk::mesh::Entity element2,
        int element1_side_id)
{
    stk::mesh::EntityId element1_global_id = bulkData.identifier(element1);
    stk::mesh::EntityId element2_global_id = bulkData.identifier(element2);
    stk::mesh::EntityId side_global_id = 0;

    if(element1_global_id < element2_global_id)
    {
        side_global_id = get_element_side_multiplier() * element1_global_id + element1_side_id;
    }
    else
    {
        int side_id = elementGraph.get_side_from_element1_to_locally_owned_element2(element2, element1);
        ThrowRequireMsg(side_id != -1, "Program error. Contact sierra-help@sandia.gov for support.");
        side_global_id = get_element_side_multiplier() * element2_global_id + side_id;
    }

    return side_global_id;
}

void filter_for_candidate_elements_to_connect(const stk::mesh::BulkData & mesh,
                                              const stk::mesh::Entity localElement,
                                              const unsigned sideOrdinal,
                                              ConnectedElementDataVector & connectedElementData)
{
    ConnectedElementDataVector filteredConnectedElements;
    stk::topology localElemTopology = mesh.bucket(localElement).topology();
    stk::topology localFaceTopology = localElemTopology.face_topology(sideOrdinal);
    bool foundEquivalent = false;

    if (localElemTopology.is_shell()) {
        // Make sure we only try to connect to volume elements on the designated
        // side of the shell currently being processed, since elements on
        // both sides share all of the same shell nodes.  Match with volume element
        // sides that have opposite polarity only and ignore coincident shells.
        //
        for (const ConnectedElementData & connectedElem: connectedElementData) {
            stk::mesh::OrdinalAndPermutation localElemOrdAndPerm = stk::mesh::get_ordinal_and_permutation(mesh, localElement, mesh.mesh_meta_data().side_rank(), connectedElem.m_sideNodes);
            bool localNegativeRelativeFacePolarity = (localElemOrdAndPerm.first != sideOrdinal);

            if (!connectedElem.m_elementTopology.is_shell() && localNegativeRelativeFacePolarity) {
                filteredConnectedElements.push_back(connectedElem);
            }
        }
        connectedElementData.swap(filteredConnectedElements);
    }
    else {
        // The element being processed is not a shell, so we need to check
        // the list of connected elements to see if any adjacent shells
        // exist, because they will "block" direct connections to volume
        // elements that also share the same side nodes.
        //
        for (const ConnectedElementData & connectedElem: connectedElementData) {
            const stk::mesh::Entity* localElemNodes = mesh.begin_nodes(localElement);
            stk::mesh::EntityVector localElemSideNodes(connectedElem.m_sideNodes.size());
            localElemTopology.side_nodes(localElemNodes, sideOrdinal, localElemSideNodes.begin());

            std::pair<bool,unsigned> result = localFaceTopology.equivalent(localElemSideNodes, connectedElem.m_sideNodes);
            const bool isEquivalentNodes = result.first;
            foundEquivalent = foundEquivalent || isEquivalentNodes;

            if (connectedElem.m_elementTopology.is_shell() && isEquivalentNodes) {
                stk::mesh::OrdinalAndPermutation localElemOrdAndPerm = stk::mesh::get_ordinal_and_permutation(mesh, localElement, mesh.mesh_meta_data().side_rank(), connectedElem.m_sideNodes);
                bool localNegativeRelativeFacePolarity = !localFaceTopology.is_positive_polarity(localElemOrdAndPerm.second);

                if (localNegativeRelativeFacePolarity) {
                    filteredConnectedElements.push_back(connectedElem);
                }
            }
        }

        if (!filteredConnectedElements.empty()) {
            // Found some shells, so ignore everything else and just connect to
            // these shells.  Otherwise, leave the original list of connected
            // elements intact.
            //
            connectedElementData.swap(filteredConnectedElements);
        }

        if (!foundEquivalent) {
            // Nothing matches at all, so flush the list of candidate elements
            connectedElementData.clear();
        }
    }
}

void pack_newly_shared_remote_edges(stk::CommSparse &comm, const stk::mesh::BulkData &bulkData, const std::vector<SharedEdgeInfo> &newlySharedEdges)
{
    std::vector<SharedEdgeInfo>::const_iterator iter = newlySharedEdges.begin();
    std::vector<SharedEdgeInfo>::const_iterator endIter = newlySharedEdges.end();

    for(; iter!= endIter; ++iter)
    {
        stk::mesh::EntityId localId = iter->m_locaElementlId;
        stk::mesh::Entity localEntity = bulkData.get_entity(stk::topology::ELEM_RANK, localId);
        stk::mesh::EntityId remoteId = iter->m_remoteElementId;
        unsigned side_index    = iter->m_sideIndex;
        int sharing_proc       = iter->m_procId;
        stk::mesh::EntityId chosenId = iter->m_chosenSideId;
        const bool isInPart = iter->m_isInPart;

        size_t numNodes= iter->m_sharedNodes.size();
        std::vector<stk::mesh::EntityKey> side_node_entity_keys(numNodes);
        for(size_t i=0; i<numNodes; ++i)
        {
            side_node_entity_keys[i] = bulkData.entity_key(iter->m_sharedNodes[i]);
        }

        comm.send_buffer(sharing_proc).pack<stk::mesh::EntityId>(localId);
        comm.send_buffer(sharing_proc).pack<stk::mesh::EntityId>(remoteId);
        comm.send_buffer(sharing_proc).pack<unsigned>(side_index);
        comm.send_buffer(sharing_proc).pack<stk::mesh::EntityId>(chosenId);
        comm.send_buffer(sharing_proc).pack<bool>(isInPart);
        comm.send_buffer(sharing_proc).pack<stk::topology>(bulkData.bucket(localEntity).topology());
        comm.send_buffer(sharing_proc).pack<unsigned>(numNodes);
        for(size_t i=0; i<numNodes; ++i)
        {
            comm.send_buffer(sharing_proc).pack<stk::mesh::EntityKey>(side_node_entity_keys[i]);
        }
    }
}

}}} // end namespaces stk mesh impl

