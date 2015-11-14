#ifndef STK_ELEM_ELEM_GRAPH_HPP
#define STK_ELEM_ELEM_GRAPH_HPP

#include <vector>
#include <map>
#include <stk_topology/topology.hpp>
#include <stk_mesh/base/Entity.hpp>
#include <stk_mesh/base/Types.hpp>

#include "ElemElemGraphImpl.hpp"

namespace stk { class CommBuffer; }

namespace stk { namespace mesh { class BulkData; } }

namespace stk { namespace mesh {

struct moved_parallel_graph_info {

    moved_parallel_graph_info(int in_proc_to_tell, stk::mesh::EntityId in_elem_id, stk::mesh::EntityId in_moved_elem_id, int in_destination_proc)
     : proc_to_tell(in_proc_to_tell), elem_id(in_elem_id), moved_elem_id(in_moved_elem_id), destination_proc(in_destination_proc) {};

    int proc_to_tell;
    stk::mesh::EntityId elem_id;
    stk::mesh::EntityId moved_elem_id;
    int destination_proc;
};

void change_entity_owner(stk::mesh::BulkData &bulkData, stk::mesh::ElemElemGraph &elem_graph,
                         std::vector< std::pair< stk::mesh::Entity, int > > &elem_proc_pairs_to_move,
                         stk::mesh::Part *active_part=NULL);

class ElemElemGraph
{
public:
    ElemElemGraph(stk::mesh::BulkData& bulkData, const stk::mesh::Part &part);

    virtual ~ElemElemGraph();

    size_t get_num_connected_elems(stk::mesh::Entity local_element) const;

    bool is_connected_elem_locally_owned(stk::mesh::Entity local_element, size_t index_conn_elem) const;

    stk::mesh::Entity get_connected_element(stk::mesh::Entity local_element, size_t index_conn_elem) const;

    stk::mesh::EntityId get_entity_id_of_remote_element(stk::mesh::Entity local_element, size_t index_conn_elem) const;

    int get_owning_proc_id_of_remote_element(stk::mesh::Entity local_element, stk::mesh::EntityId other_element_id) const;

    int get_side_id_to_connected_element(stk::mesh::Entity local_element, size_t index_conn_elem) const;

    int get_side_from_element1_to_remote_element2(stk::mesh::Entity local_element, stk::mesh::EntityId other_element_id) const;

    int get_side_from_element1_to_locally_owned_element2(stk::mesh::Entity local_element, stk::mesh::Entity other_element) const;

    bool is_connected_to_other_element_via_side_ordinal(stk::mesh::Entity element, int sideOrdinal) const;

    impl::parallel_info& get_parallel_edge_info(stk::mesh::Entity element, stk::mesh::EntityId remote_id);

    size_t num_edges() const;

    size_t num_parallel_edges() const { return m_num_parallel_edges; }

    std::vector<stk::mesh::EntityId> get_suggested_side_ids() const;

    void set_num_side_ids_used(size_t num_used);

    void add_elements_to_graph(const stk::mesh::EntityVector &elements_to_add);

    void delete_elements_from_graph(const stk::mesh::EntityVector &elements_to_delete);

    bool is_valid_graph_element(stk::mesh::Entity local_element);

    size_t size() {return m_elem_graph.size() - m_deleted_element_local_id_pool.size();}

    impl::LocalId get_local_element_id(stk::mesh::Entity local_element, bool require_valid_id = true) const;

protected:
    friend void change_entity_owner(stk::mesh::BulkData &bulkData, stk::mesh::ElemElemGraph &elem_graph,
                                    std::vector< std::pair< stk::mesh::Entity, int > > &elem_proc_pairs_to_move,
                                    stk::mesh::Part *active_part);

    //this member method is not the public API for change-entity-owner. see the free-standing function above
    void change_entity_owner(const stk::mesh::EntityProcVec &elem_proc_pairs_to_move, impl::ParallelGraphInfo &parallel_graph_info, stk::mesh::Part *active_part=NULL);

    void fill_graph();
    void update_number_of_parallel_edges();
    void fill_parallel_graph(impl::ElemSideToProcAndFaceId& elem_side_comm, const stk::mesh::Part &part);

    void fill_parallel_graph(impl::ElemSideToProcAndFaceId& elem_side_comm, const stk::mesh::EntityVector & elements_to_ignore);

    void add_possibly_connected_elements_to_graph_using_side_nodes( const stk::mesh::impl::ElemSideToProcAndFaceId& elemSideComm,
                                                                    stk::mesh::impl::ConnectedElementDataVector & communicatedElementDataVector,
                                                                    const stk::mesh::EntityVector & elements_to_ignore,
                                                                    std::vector<impl::SharedEdgeInfo> &newlySharedEdges);

    void  break_local_volume_element_connections_across_shells(const std::set<stk::mesh::EntityId> & localElementsConnectedToRemoteShell);

    void break_remote_volume_element_connections_across_shells(const std::vector< std::pair< stk::mesh::Entity, stk::mesh::EntityId > > & localAndRemoteElementsConnectedToShell);

    void add_local_elements_to_connected_list(const stk::mesh::EntityVector & connected_elements,
                                              const stk::mesh::EntityVector & sideNodes,
                                              impl::ConnectedElementDataVector & connectedElementDataVector) const;

    void get_element_side_pairs(const stk::mesh::MeshIndex &meshIndex, impl::LocalId local_elem_id, std::vector<impl::ElementSidePair> &elem_side_pairs) const;

    stk::mesh::ConnectivityOrdinal get_neighboring_side_ordinal(const stk::mesh::BulkData &mesh, stk::mesh::Entity currentElem,
                                                                stk::mesh::ConnectivityOrdinal currentOrdinal, stk::mesh::Entity neighborElem);

    impl::LocalId create_new_local_id(stk::mesh::Entity new_elem);

    impl::LocalId get_new_local_element_id_from_pool();
    int size_data_members();
    void ensure_space_in_entity_to_local_id(size_t max_index);
    size_t find_max_local_offset_in_neighborhood(stk::mesh::Entity element);
    void pack_deleted_element_comm(stk::CommSparse &comm,
                                   const std::vector<impl::DeletedElementData> &local_elem_and_remote_connected_elem);

    void pack_remote_connected_element(impl::LocalId elem_local_id, stk::mesh::EntityId connected_global_id,
                                                      stk::CommBuffer &buff, std::vector<moved_parallel_graph_info> &moved_graph_info_vector,
                                                      int destination_proc, int phase);

    void pack_local_connected_element(impl::LocalId local_id, int side_id, stk::CommBuffer &buff,
                                                     stk::mesh::EntityId suggested_face_id,
                                                     stk::mesh::Part *active_part);

    void pack_shell_connectivity(stk::CommSparse & comm, const std::vector<impl::ShellConnectivityData> & shellConnectivityList);

    void pack_remote_edge_across_shell(stk::CommSparse &comm, stk::mesh::EntityVector &addedShells, int phase);

    void unpack_and_store_connected_element(stk::CommBuffer &buf, impl::LocalId recvd_elem_local_id,
                                                           stk::mesh::EntityId recvd_elem_global_id);

    void communicate_moved_graph_info(std::vector <moved_parallel_graph_info> &moved_graph_info_vector);

    void filter_for_elements_in_graph(stk::mesh::EntityVector &localElements);

    void communicate_remote_edges_for_pre_existing_graph_nodes(const std::vector<impl::SharedEdgeInfo> &newlySharedEdges,
                                                          std::vector<impl::SharedEdgeInfo> &receivedSharedEdges);

    void connect_remote_element_to_existing_graph(const impl::SharedEdgeInfo &receivedSharedEdge);

    void generate_additional_ids_collective(size_t num_additional_ids_needed);

    void collect_local_shell_connectivity_data(const stk::mesh::EntityVector& elements_to_delete,
                                               std::vector<impl::ShellConnectivityData>& shellConnectivityList);

    void communicate_shell_connectivity(std::vector<impl::ShellConnectivityData>& shellConnectivityList);

    void delete_local_connections_and_collect_remote(const stk::mesh::EntityVector& elements_to_delete,
                                                     std::vector<impl::DeletedElementData>& local_elem_and_remote_connected_elem);

    void communicate_remote_connections_to_delete(const std::vector<impl::DeletedElementData>& local_elem_and_remote_connected_elem,
                                                  std::vector<std::pair<stk::mesh::EntityId, stk::mesh::EntityId> >& remote_edges);

    void clear_deleted_element_connections(const stk::mesh::EntityVector& elements_to_delete);

    void delete_remote_connections(const std::vector<std::pair<stk::mesh::EntityId, stk::mesh::EntityId> >& remote_edges);

    void reconnect_volume_elements_across_deleted_shells(std::vector<impl::ShellConnectivityData> & shellConnectivityList,
                                                         const stk::mesh::EntityVector& elements_to_delete);

    void break_remote_shell_connectivity_and_pack(stk::CommSparse& comm, impl::LocalId leftId, impl::LocalId rightId, int phase);

    void pack_both_remote_shell_connectivity(stk::CommSparse &comm, impl::LocalId shellId, impl::LocalId leftId, impl::LocalId rightId);

    void unpack_remote_edge_across_shell(stk::CommSparse &comm);

    stk::mesh::BulkData &m_bulk_data;
    const stk::mesh::Part &m_part;
    impl::ElementGraph m_elem_graph;
    impl::SidesForElementGraph m_via_sides;
    impl::ParallelGraphInfo m_parallel_graph_info;
    stk::mesh::EntityVector m_local_id_to_element_entity;
    std::vector<impl::LocalId> m_entity_to_local_id;
    std::vector<impl::LocalId> m_deleted_element_local_id_pool;
    std::vector<bool> m_local_id_in_pool;
    std::vector<stk::topology> m_element_topologies;
    std::vector<stk::mesh::EntityId> m_suggested_side_ids;
    std::vector<int> m_deleted_elem_pool;
    size_t m_num_edges;
    size_t m_num_parallel_edges;
    size_t m_num_ids_used;

    static const impl::LocalId INVALID_LOCAL_ID;

};

bool perform_element_death(stk::mesh::BulkData& bulkData, ElemElemGraph& elementGraph, const stk::mesh::EntityVector& killedElements, stk::mesh::Part& active,
        const stk::mesh::PartVector& boundary_mesh_parts);

}} // end stk mesh namespaces

#endif
