#pragma once

#include <map>
#include <stack>

#include "../Node.h"
#include "CFG_parts.h"
#include "backend/JIR/ScopeManager.h"


namespace eraxc::JIR {

    typedef std::vector<Node> Nodes;

    class CFG {
        std::vector<CFG_Node> nodes;

        // multimap <int from_id, int to_id>. every JUMP is an edge
        std::multimap<size_t, size_t> edges;

        std::map<u64, CFG_Func> global_funcs;
        ScopeManager scopeManager;

        std::stack<Operation> jump_ops;

        error::errable<void> parse_declaration(const std::vector<token>& tokens, int& i, size_t node_id);
        error::errable<void> parse_function(const std::vector<token>& tokens, int& i, size_t& node_id);
        error::errable<void> parse_statements(const std::vector<token>& tokens, int& i, size_t& node_id);
        error::errable<void> parse_statement(const std::vector<token>& tokens, int& i, size_t& node_id);

        error::errable<void> parse_if(const std::vector<token>& tokens, int& i, size_t& node_id);
        error::errable<void> parse_do(const std::vector<token>& tokens, int& i, size_t node_id);
        error::errable<void> parse_while(const std::vector<token>& tokens, int& i, size_t node_id);
        error::errable<void> parse_for(const std::vector<token>& tokens, int& i, size_t node_id);

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

        void print_functions() const;
        void print_nodes() const;


        const std::vector<CFG_Node>& get_nodes() const {
            return nodes;
        };

        /// Eliminates all the nodes that aren't used from CFG
        void dead_code_elimination_pass();
    };
}
