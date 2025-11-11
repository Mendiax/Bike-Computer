""" Program to convert a C/C++ struct to a string representation.
This program defines a function that takes a C/C++ struct definition as input
and generates a string representation of the struct's fields and their values.

Usage:
    python struct_to_string.py <struct_name> <file_path>


Workflow:

Find strcut definition in the given file.
Check for namespace if any.

Generate a tags at the bottom of the file with the following format:
#ifndef AUTOGEN_TO_STRING_<STRUCT_NAME>
static inline std::string to_string(const <STRUCT_NAME>& obj) {
    ... implementation ..
}
#endif

"""


import re
import sys
import os
import json

from tree_sitter import Language, Parser, Tree, Node
import tree_sitter_cpp
from typing import Generator

def struct_to_string(struct_name: str, file_path: str, overwrite : bool) -> str:
    with open(file_path, 'r') as file:
        content = file.read()
        # src_lines = file.readlines()

    parser = Parser(Language(tree_sitter_cpp.language()))

    # def read_callable_point(byte_offset, point):
    #     row, column = point
    #     if row >= len(src_lines) or column >= len(src_lines[row]):
    #         return None
    #     return src_lines[row][column:].encode("utf8")


    # tree = parser.parse(read_callable_point, encoding="utf8")
    # print(tree.walk().node)

    tree = parser.parse(bytes(content, "utf8"))
    def traverse_tree(tree: Tree) -> Generator[Node, None, None]:
        cursor = tree.walk()

        visited_children = False
        while True:
            if not visited_children:
                yield cursor.node
                if not cursor.goto_first_child():
                    visited_children = True
            elif cursor.goto_next_sibling():
                visited_children = False
            elif not cursor.goto_parent():
                break


    # Build a dict: {namespace: [ {name: struct_name, members: [(type, name)]} ]}

    def get_namespaces_and_structs_with_members(tree: Tree):
        namespaces = {}
        stack = [('', tree.root_node)]  # (current_namespace, node)
        while stack:
            current_ns, node = stack.pop()
            if node.type == 'namespace_definition':
                # Get namespace name
                ns_name_node = node.child_by_field_name('name')
                if ns_name_node:
                    ns_name_val = ns_name_node.text
                    if isinstance(ns_name_val, bytes):
                        ns_name = ns_name_val.decode()
                    else:
                        ns_name = str(ns_name_val)
                else:
                    ns_name = ''
                new_ns = f"{current_ns}::{ns_name}" if current_ns else ns_name
                # Descend into namespace body
                body = node.child_by_field_name('body')
                if body:
                    stack.append((str(new_ns), body))
            elif node.type == 'struct_specifier':
                # Get struct name
                struct_name_node = node.child_by_field_name('name')
                if struct_name_node:
                    struct_name_val = struct_name_node.text
                    if isinstance(struct_name_val, bytes):
                        struct_name = struct_name_val.decode()
                    else:
                        struct_name = str(struct_name_val)
                else:
                    struct_name = ''
                # Get member (type, name) tuples
                members = []
                field_node = node.child_by_field_name('body')
                if field_node:
                    for child in field_node.children:
                        # Only look for field_declaration nodes
                        if child.type == 'field_declaration':
                            # The field type is a child with field_name 'type'
                            type_node = child.child_by_field_name('type')
                            # The field name is a child with field_name 'declarator'
                            declarator = child.child_by_field_name('declarator')
                            if declarator:
                                name_val = declarator.text
                                if isinstance(name_val, bytes):
                                    member_name = name_val.decode()
                                else:
                                    member_name = str(name_val)
                            else:
                                member_name = ''
                            # Check for nameless struct type
                            if type_node and type_node.type == 'struct_specifier':
                                # If the struct_specifier has no name, use member_name with prefix
                                inner_struct_name_node = type_node.child_by_field_name('name')
                                if inner_struct_name_node is None or not inner_struct_name_node.text:
                                    # Use member_name with prefix as struct name
                                    inner_struct_name = f"__anon_{member_name}" if member_name else "__anon_struct"
                                else:
                                    inner_struct_val = inner_struct_name_node.text
                                    if isinstance(inner_struct_val, bytes):
                                        inner_struct_name = inner_struct_val.decode()
                                    else:
                                        inner_struct_name = str(inner_struct_val)
                                member_type = f"struct {inner_struct_name}"
                            elif type_node:
                                type_val = type_node.text
                                if isinstance(type_val, bytes):
                                    member_type = type_val.decode()
                                else:
                                    member_type = str(type_val)
                            else:
                                member_type = ''
                            members.append((member_type, member_name))
                ns_key = str(current_ns) if current_ns else '::'
                namespaces.setdefault(ns_key, []).append({'name': struct_name, 'members': members})
            # Traverse children
            for child in reversed(list(node.children)):
                # Avoid descending into namespace body twice
                if node.type == 'namespace_definition' and child == node.child_by_field_name('body'):
                    continue
                stack.append((current_ns, child))
        return namespaces

    ns_structs = get_namespaces_and_structs_with_members(tree)
    # print(json.dumps(ns_structs, indent=2))
    # get all names of structures in the file

    if("::" in struct_name):
        namespace, name = struct_name.rsplit("::", 1)
    else:
        namespace = '::' # global namespace (no namespace)
        name = struct_name
    print(namespace, name)

    strucures_in_ns = ns_structs.get(namespace, [])
    if not strucures_in_ns:
        raise ValueError(f"Namespace {namespace} not found in {file_path}")

    # print(json.dumps(strucures_in_ns, indent=2))

    def get_structure_by_name(structures, name):
        for struct in structures:
            if struct['name'] == name:
                return struct
        return None

    struct_info = get_structure_by_name(strucures_in_ns, name)
    if not struct_info:
        raise ValueError(f"Struct {name} not found in namespace {namespace} in {file_path}")

    print(json.dumps(struct_info, indent=2))

    members = struct_info['members']

    def get_member_tree(members, prefix=""):
        tree = {}
        for field_type, field_name in members:
            if field_type.startswith("struct "):
                print(f"struct {field_name} ")
                sub_members = get_structure_by_name(strucures_in_ns, field_type.lstrip("struct "))
                parsed_members = get_member_tree(sub_members['members'], prefix=prefix + field_name + ".")
                tree[field_name] = parsed_members
            else:
                print(f"{field_type} {prefix}{field_name};")
                tree[f'{prefix}{field_name}'] = field_type
        return tree

    tree = get_member_tree(members)

    print("Final tree:")
    print(json.dumps(tree, indent=2))
    function = generate_to_string_function_yaml(struct_name, tree)
    write_function_to_file(function, struct_name, file_path, overwrite)

    function = generate_to_string_function(struct_name, tree)
    write_function_to_file(function, struct_name, file_path, overwrite)
    return function

def generate_to_string_function_yaml(struct_name: str, tree: dict) -> str:
    TYPE = '_yaml'
    function = fr"""#ifndef AUTOGEN_TO_STRING_{struct_name.upper().replace("::", "_")}{TYPE.upper()}
static inline std::string to_string{TYPE}(const {struct_name}& obj) {{
    std::stringstream ss;
"""
    function += "    ss << \"Struct {struct_name} contents:\\n\";\n"
    INDENT = "    "
    def generate_string_representation(tree, prefix="obj.", indent=0):
        lines = []
        longest_member_length = max(len(member.split('.')[-1]) for member in tree.keys()) + 1
        for member, mtype in tree.items():
            if isinstance(mtype, dict):
                lines.append(f'    ss << "- {INDENT * indent}{member}: " << "\\n";')
                lines.extend(generate_string_representation(mtype, prefix=prefix, indent=indent + 1))
            else:
                lines.append(f'    ss << "{INDENT * indent}{member.split('.')[-1]+ ':':<{longest_member_length}} " << {prefix}{member} << "\\n";')
        return lines
    function_lines = generate_string_representation(tree)
    function += "\n".join(function_lines)
    function += '\nreturn ss.str();\n}\n#endif\n'
    print("Generated function:")
    print(function)
    return function

def generate_to_string_function(struct_name: str, tree: dict) -> str:
    TYPE = ''
    function = fr"""#ifndef AUTOGEN_TO_STRING_{struct_name.upper().replace("::", "_")}{TYPE.upper()}
static inline std::string to_string{TYPE}(const {struct_name}& obj) {{
    std::stringstream ss;
"""
    function += f"    ss << \"{struct_name}:{{\";\n"
    INDENT = "    "
    def generate_string_representation(tree, prefix="obj.", indent=0):
        lines = []
        longest_member_length = max(len(member.split('.')[-1]) for member in tree.keys()) + 1
        for member, mtype in tree.items():
            if isinstance(mtype, dict):
                lines.append(f'    ss << " {member}{{";')
                lines.extend(generate_string_representation(mtype, prefix=prefix, indent=indent + 1))
                lines.append(f'    ss << "}}";')
            else:
                lines.append(f'    ss << " {member.split('.')[-1]+ '='}" << {prefix}{member};')
        return lines
    function_lines = generate_string_representation(tree)
    function += "\n".join(function_lines)
    function += '\n    ss << \"}\";\n'
    function += '\nreturn ss.str();\n}\n#endif\n'
    print("Generated function:")
    print(function)
    return function

def write_function_to_file(function: str, struct_name: str, file_path: str, overwrite: bool):
    with open(file_path, 'r') as file:
        content = file.read()
    # check for existing to_string function in the file
    function_first_line = function.splitlines()[0]
    if function_first_line in content:
        if overwrite:
            print(f"Overwriting existing to_string function for {struct_name} in {file_path}")
            # find first line of existing function
            pattern = re.compile(function_first_line, re.MULTILINE)
            match = pattern.search(content)
            if match:
                start_index = match.start()
                # find the end of the function (next #endif)
                end_pattern = re.compile(r"#endif\n")
                end_match = end_pattern.search(content, pos=start_index)
                if end_match:
                    end_index = end_match.end()
                    # replace the existing function
                    new_content = content[:start_index] + function + content[end_index:]
                    with open(file_path, 'w') as file:
                        file.write(new_content)
                    return function
                else:
                    raise ValueError(f"Could not find the end of the existing to_string function for {struct_name} in {file_path}")
        else:
            raise ValueError(f"to_string function for {struct_name} already exists in {file_path}")

    with open(file_path, 'a') as file:
        file.write("\n" + function)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='Convert C/C++ struct to string representation.')
    parser.add_argument('struct_name', type=str, help='Name of the struct to convert.')
    parser.add_argument('file_path', type=str, help='Path to the C/C++ file containing the struct definition.')
    parser.add_argument('--overwrite', action='store_true', help='Overwrite existing to_string function if it exists.')
    args = parser.parse_args()

    result = struct_to_string(args.struct_name, args.file_path, args.overwrite)
    print(result)