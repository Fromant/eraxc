import argparse
from graphviz import Digraph

def parse_cfg_file(file_path):
    """
    Parse CFG edge data from a file.
    Expected format per line: from_id, to_id, edge_type

    Args:
        file_path: Path to input file containing CFG edges

    Returns:
        List of tuples (from_id, to_id, edge_type)
    """
    edges = []
    with open(file_path, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            if not line or line.startswith('#'):
                continue  # Skip empty lines and comments

            try:
                parts = [p.strip() for p in line.split(',')]
                if len(parts) != 3:
                    raise ValueError(f"Expected 3 values, got {len(parts)}")

                from_id = int(parts[0])
                to_id = int(parts[1])
                edge_type = int(parts[2])

                if edge_type not in (0, 1):
                    raise ValueError(f"Invalid edge type {edge_type} (must be 0 or 1)")

                edges.append((from_id, to_id, edge_type))
            except Exception as e:
                raise ValueError(f"Error parsing line {line_num} in file '{file_path}': {str(e)}")

    if not edges:
        raise ValueError("No valid edges found in input file")

    return edges

def generate_cfg_image(edges, output_path="cfg_graph.png"):
    """
    Generate a Control Flow Graph image from edge data.

    Args:
        edges: List of tuples (from_id, to_id, edge_type)
               edge_type: 0 = "squash", 1 = "extend"
        output_path: Path to save the output image (PNG format)
    """
    dot = Digraph(comment='Control Flow Graph',
                  format='png',
                  graph_attr={
                      'rankdir': 'TB',       # Top-to-bottom layout
                      'fontsize': '12',
                      'fontname': 'Helvetica',  # Clean modern font
                      'splines': 'ortho',    # Orthogonal edges for readability
                      'nodesep': '0.8',
                      'ranksep': '1.0',
                      'concentrate': 'true',  # Merge parallel edges
                      'bgcolor': '#FFFFFF00'  # Transparent background
                  },
                  node_attr={
                      'shape': 'circle',
                      'style': 'filled,rounded',
                      'fillcolor': '#E8F4F8',
                      'color': '#2A5B7F',
                      'fontname': 'Helvetica',
                      'fontsize': '11',
                      'width': '0.8',
                      'height': '0.8',
                      'penwidth': '1.2'
                  },
                  edge_attr={
                      'fontname': 'Helvetica',
                      'fontsize': '10',
                      'arrowsize': '0.8',
                      'arrowhead': 'normal',
                      'penwidth': '1.5'
                  })

    nodes = {}

    for from_id, to_id, edge_type in edges:
        src = str(from_id)
        dst = str(to_id)

        nodes.setdefault(src, []).append(('out', edge_type))
        nodes.setdefault(dst, []).append(('in', edge_type))

        edge_info = {
            0: ("squash", "#FF6B6B", "#FFA9A9"),  # Red tones for squash
            1: ("extend", "#4ECDC4", "#A9E7E0")   # Teal tones for extend
        }
        label, color, fontcolor = edge_info[edge_type]

        dot.edge(src, dst,
                 label=label,
                 color=color,
                 fontcolor=fontcolor,
                 labelfontsize='10',
                 labelfontname='Helvetica-Bold')

    for node_id, connections in nodes.items():
        has_squash = any(edge_type == 0 for _, edge_type in connections)
        has_extend = any(edge_type == 1 for _, edge_type in connections)

        if has_squash and has_extend:
            fillcolor = '#FFE066'  # Yellow for mixed nodes
        elif has_squash:
            fillcolor = '#FFB3B3'  # Light red for squash-only
        elif has_extend:
            fillcolor = '#B3E5E5'  # Light teal for extend-only
        else:
            fillcolor = '#E8F4F8'  # Default

        dot.node(node_id,
                 node_id,
                 fillcolor=fillcolor,
                 color='#2A5B7F',
                 fontcolor='#1A3C5A')

    all_froms = {str(e[0]) for e in edges}
    all_tos = {str(e[1]) for e in edges}

    start_nodes = all_froms - all_tos
    for node in start_nodes:
        dot.node(node, node,
                 shape='doublecircle',
                 fillcolor='#C3F0CA',
                 color='#2E7D32',
                 penwidth='2.0')

    # End nodes (only incoming edges)
    end_nodes = all_tos - all_froms
    for node in end_nodes:
        dot.node(node, node,
                 shape='doublecircle',
                 fillcolor='#FFCCBC',
                 color='#C62828',
                 penwidth='2.0')

    dot.render(output_path, cleanup=True)
    print(f"Control Flow Graph saved to: {output_path}")
    print(f"Generated from {len(edges)} edges connecting {len(nodes)} nodes")

def main():
    parser = argparse.ArgumentParser(description='Generate CFG visualization from edge data')
    parser.add_argument('input_file', help='Input file with CFG edges (format: from_id, to_id, edge_type)')
    parser.add_argument('-o', '--output', default='cfg_graph.png',
                        help='Output image path (default: cfg_graph.png)')
    parser.add_argument('--view', action='store_true',
                        help='Open the generated image after creation')

    args = parser.parse_args()

    try:
        edges = parse_cfg_file(args.input_file)
        generate_cfg_image(edges, args.output)

        if args.view:
            import os
            os.startfile(args.output) if os.name == 'nt' else os.system(f'open {args.output}')

    except Exception as e:
        print(f"Error: {str(e)}")
        exit(1)

if __name__ == "__main__":
    main()