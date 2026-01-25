
# This tool is created to generate png images of CFG

## Deps:
- Graphviz (`https://graphviz.org/download/` (needs to be in PATH) and `pip install graphviz`)

## Usage: 
`py CFGViewer.py <input file path> [--view] [-o (or --output) <output file path>]>`

`--view` flag opens diagram on your screen after generating it.

### Input file format:
Input file consist of any number of `start_id, end_id, type` lines

Type determines if edge is `squash` `(type = 0)` or `extend` `(type = 1)`. This will be rendered on image as text near to edges

Lines starting with `#` will be ignored