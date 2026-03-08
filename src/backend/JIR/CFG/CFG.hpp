#pragma once

#include <map>
#include <stack>

#include "CFG_parts.hpp"
#include "backend/JIR/ScopeManager.hpp"


namespace eraxc::JIR {
    using Nodes = std::vector<JIRop>;
    using EdgesMap = std::unordered_map<size_t, std::vector<CFGEdge>>;

    class CFG {
        std::vector<CFG_Node> nodes;
        size_t global_node_id = 0;
        std::vector<Scope::Declaration> global_decls;

        EdgesMap edges;

        void appendEdge(size_t from, CFGEdge edge) {
            if (const auto it = edges.find(from); it == edges.end()) {
                edges.emplace(from, std::vector {edge});
            } else {
                it->second.push_back(edge);
            }
        }

        std::map<u64, CFG_Func> global_funcs;
        ScopeManager scopeManager;

        std::stack<Operation> jump_ops;

        error::errable<void> parse_declaration(const std::vector<token>& tokens, int& i, size_t node_id);
        error::errable<void> parse_function(const std::vector<token>& tokens, int& i, const size_t& node_id);
        error::errable<void> parse_statements(const std::vector<token>& tokens, int& i, size_t& node_id);
        error::errable<void> parse_statement(const std::vector<token>& tokens, int& i, size_t& node_id);

        error::errable<void> parse_if(const std::vector<token>& tokens, int& i, size_t& node_id);
        error::errable<void> parse_do(const std::vector<token>& tokens, int& i, size_t& node_id);
        error::errable<void> parse_while(const std::vector<token>& tokens, int& i, size_t& node_id);
        error::errable<void> parse_for(const std::vector<token>& tokens, int& i, size_t& node_id);

        error::errable<Operand> parse_instant(const token& t) const;
        error::errable<Operand> parse_expression(const std::vector<token>& tokens, int& i, size_t& node_id,
                                                 const std::set<token::type>& end = {token::SEMICOLON});
        error::errable<void> push_expr_stack(std::stack<syntax::operator_type>& operations,
                                             std::stack<Operand>& operands, size_t& node_id);
        error::errable<std::pair<Operand, Nodes>> parse_expr_operand(const std::vector<token>& tokens, int& i,
                                                                     size_t node_id);


    public:
        /// Builds full CFG from token stream, including main() detection and etc
        /// @param tokens Tokens to make CFG from
        /// @return error that happened if any did
        error::errable<void> create(const std::vector<token>& tokens);

        const CFG_Node& get_cfg_node(size_t node_id) const {
            return nodes[node_id];
        };
        const std::map<u64, CFG_Func>& get_funcs() const {
            return global_funcs;
        }
        const auto& get_edges() const {
            return edges;
        }

        const ScopeManager& getScopeManager() const {
            return scopeManager;
        }

        const auto& getGlobals() const {
            return global_decls;
        }

        void print_functions() const;
        void print_nodes() const;


        const std::vector<CFG_Node>& get_nodes() const {
            return nodes;
        };

        void print_to_file(const std::string& path) const {
            std::ofstream f(path);
            for (const auto& node_edges : edges) {
                for (const auto& edge : node_edges.second) {
                    f << node_edges.first << ", " << edge.to_id << ", " << edge.type << std::endl;
                }
            }
            f.close();
        }

        /// Eliminates all the nodes that aren't used from CFG
        void dead_code_elimination_pass();
    };
}
