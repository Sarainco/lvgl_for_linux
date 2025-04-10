import os
import sys
from xml.etree import ElementTree as ET

base_path = ''
xml_path = ''

EMIT_WARNINGS = True
DOXYGEN_OUTPUT = True

MISSING_FUNC = 'MissingFunctionDoc'
MISSING_FUNC_ARG = 'MissingFunctionArgDoc'
MISSING_FUNC_RETURN = 'MissingFunctionReturnDoc'
MISSING_FUNC_ARG_MISMATCH = 'FunctionArgMissing'
MISSING_STRUCT = 'MissingStructureDoc'
MISSING_STRUCT_FIELD = 'MissingStructureFieldDoc'
MISSING_UNION = 'MissingUnionDoc'
MISSING_UNION_FIELD = 'MissingUnionFieldDoc'
MISSING_ENUM = 'MissingEnumDoc'
MISSING_ENUM_ITEM = 'MissingEnumItemDoc'
MISSING_TYPEDEF = 'MissingTypedefDoc'
MISSING_VARIABLE = 'MissingVariableDoc'
MISSING_MACRO = 'MissingMacroDoc'


def warn(warning_type, *args):
    if EMIT_WARNINGS:
        args = ' '.join(str(arg) for arg in args)

        if warning_type is None:
            output = f'\033[31;1m    {args}\033[0m\n'
        else:
            output = f'\033[31;1m{warning_type}: {args}\033[0m\n'

        sys.stdout.write(output)
        sys.stdout.flush()


def build_docstring(element):
    docstring = None
    if element.tag == 'parameterlist':
        return None

    if element.text:
        docstring = element.text.strip()

    for item in element:
        ds = build_docstring(item)
        if ds:
            if docstring:
                docstring += ' ' + ds
            else:
                docstring = ds.strip()

    if element.tag == 'para':
        if docstring:
            docstring = '\n\n' + docstring

    if element.tag == 'ref':
        docstring = f':ref:`{docstring}`'

    if element.tail:
        if docstring:
            docstring += ' ' + element.tail.strip()
        else:
            docstring = element.tail.strip()

    return docstring


def read_as_xml(d):
    try:
        return ET.fromstring(d)
    except:  # NOQA
        return None


def load_xml(fle):
    fle = os.path.join(xml_path, fle + '.xml')

    with open(fle, 'rb') as f:
        d = f.read().decode('utf-8')

    # This code is to correct a bug in Doxygen. That bug incorrectly parses
    # a typedef and it causes an error to occur building the docs. The Error
    # doesn't stop the documentation from being generated, I just don't want
    # to see the ugly red output.
    #
    # if 'typedef void() lv_lru_free_t(void *v)' in d:
    #     d = d.replace(
    #         '<type>void()</type>\n        '
    #         '<definition>typedef void() lv_lru_free_t(void *v)</definition>',
    #         '<type>void</type>\n        '
    #         '<definition>typedef void(lv_lru_free_t)(void *v)</definition>'
    #     )
    #     with open(fle, 'wb') as f:
    #         f.write(d.encode('utf-8'))

    return ET.fromstring(d)


structures = {}
functions = {}
enums = {}
typedefs = {}
variables = {}
unions = {}
namespaces = {}
files = {}


# things to remove from description
# <para> </para>


class STRUCT_FIELD(object):

    def __init__(self, name, type, description, file_name, line_no):
        self.name = name
        self.type = type
        self.description = description
        self.file_name = file_name
        self.line_no = line_no


class STRUCT(object):
    _missing = MISSING_STRUCT
    _missing_field = MISSING_STRUCT_FIELD

    template = '''\
.. doxygenstruct:: {name}
   :project: lvgl
   :members:
   :protected-members:
   :private-members:
   :undoc-members:
'''

    def __init__(self, parent, refid, name, **_):
        if name in structures:
            self.__dict__.update(structures[name].__dict__)
        else:
            structures[name] = self
            self.parent = parent
            self.refid = refid
            self.name = name
            self.types = set()
            self._deps = None
            self.header_file = ''
            self.description = None
            self.fields = []
            self.file_name = None
            self.line_no = None

        if parent and refid:
            root = load_xml(refid)

            for compounddef in root:
                if compounddef.attrib['id'] != self.refid:
                    continue

                for child in compounddef:
                    if child.tag == 'includes':
                        self.header_file = os.path.splitext(child.text)[0]
                        continue

                    elif child.tag == 'location':
                        self.file_name = child.attrib['file']
                        self.line_no = child.attrib['line']

                    elif child.tag == 'detaileddescription':
                        self.description = build_docstring(child)

                    elif child.tag == 'sectiondef':
                        for memberdef in child:
                            t = get_type(memberdef)
                            description = None
                            name = ''
                            file_name = None
                            line_no = None

                            for element in memberdef:
                                if element.tag == 'location':
                                    file_name = element.attrib['file']
                                    line_no = element.attrib['line']

                                elif element.tag == 'name':
                                    name = element.text

                                elif element.tag == 'detaileddescription':
                                    description = build_docstring(element)

                            field = STRUCT_FIELD(name, t, description, file_name, line_no)
                            self.fields.append(field)

                            if t is None:
                                continue

                            self.types.add(t)

            if not self.description:
                warn(self._missing, self.name)
                warn(None, 'FILE:', self.file_name)
                warn(None, 'LINE:', self.line_no)
                warn(None)

            for field in self.fields:
                if not field.description:
                    warn(self._missing_field, self.name)
                    warn(None, 'FIELD:', field.name)
                    warn(None, 'FILE:', field.file_name)
                    warn(None, 'LINE:', field.line_no)
                    warn(None)

    def get_field(self, name):
        for field in self.fields:
            if field.name == name:
                return field

    @property
    def deps(self):
        if self._deps is None:
            self._deps = dict(
                typedefs=set(),
                functions=set(),
                enums=set(),
                structures=set(),
                unions=set(),
                namespaces=set(),
                variables=set(),
            )
            for type_ in self.types:
                if type_ in typedefs:
                    self._deps['typedefs'].add(typedefs[type_])
                elif type_ in structures:
                    self._deps['structures'].add(structures[type_])
                elif type_ in unions:
                    self._deps['unions'].add(unions[type_])
                elif type_ in enums:
                    self._deps['enums'].add(enums[type_])
                elif type_ in functions:
                    self._deps['functions'].add(functions[type_])
                elif type_ in variables:
                    self._deps['variables'].add(variables[type_])
                elif type_ in namespaces:
                    self._deps['namespaces'].add(namespaces[type_])
        return self._deps

    def __str__(self):
        return self.template.format(name=self.name)


class UNION(STRUCT):
    _missing = MISSING_UNION
    _missing_field = MISSING_UNION_FIELD

    template = '''\
.. doxygenunion:: {name}
   :project: lvgl
'''


def get_type(node):
    def gt(n):
        for c in n:
            if c.tag == 'ref':
                t = c.text.strip()
                break
        else:
            t = node.text.strip()

        return t.replace('*', '').replace('(', '').replace(')', '').strip()

    for child in node:
        if child.tag == 'type':
            return gt(child)


class VARIABLE(object):
    template = '''\
.. doxygenvariable:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in variables:
            self.__dict__.update(variables[name].__dict__)
        else:
            variables[name] = self
            self.parent = parent
            self.refid = refid
            self.name = name
            self.description = None
            self.type = ''
            self.file_name = None
            self.line_no = None

        if parent is not None:
            root = load_xml(parent.refid)

            for compounddef in root:
                if compounddef.attrib['id'] != parent.refid:
                    continue

                for child in compounddef:
                    if (
                        child.tag == 'sectiondef' and
                        child.attrib['kind'] == 'var'
                    ):
                        for memberdef in child:
                            if memberdef.attrib['id'] == refid:
                                break
                        else:
                            continue

                        self.type = get_type(memberdef)

                        for element in memberdef:
                            if element.tag == 'location':
                                self.file_name = element.attrib['file']
                                self.line_no = element.attrib['line']
                            elif element.tag == 'detaileddescription':
                                self.description = build_docstring(element)

            if not self.description:
                warn(MISSING_VARIABLE, self.name)
                warn(None, 'FILE:', self.file_name)
                warn(None, 'LINE:', self.line_no)
                warn(None)

    def __str__(self):
        return self.template.format(name=self.name)


class NAMESPACE(object):
    template = '''\
.. doxygennamespace:: {name}
   :project: lvgl
   :members:
   :protected-members:
   :private-members:
   :undoc-members:
'''

    def __init__(self, parent, refid, name, **_):
        if name in namespaces:
            self.__dict__.update(namespaces[name].__dict__)
        else:
            namespaces[name] = self
            self.parent = parent
            self.refid = refid
            self.name = name
            self.description = None
            self.line_no = None
            self.file_name = None
            self.enums = []
            self.funcs = []
            self.vars = []
            self.typedefs = []
            self.structs = []
            self.unions = []
            self.classes = []

        # root = load_xml(refid)
        #
        # for compounddef in root:
        #     if compounddef.attrib['id'] != refid:
        #         continue
        #
        #     for sectiondef in compounddef:
        #         if sectiondef.tag != 'sectiondef':
        #             continue
        #
        #         enum
        #         typedef
        #         func
        #         struct
        #         union
        #
        #
        #         cls = globals()[sectiondef.attrib['kind'].upper()]
        #         if cls == ENUM:
        #             if sectiondef[0].text:
        #                 sectiondef.attrib['name'] = sectiondef[0].text.strip()
        #                 enums_.append(cls(self, **sectiondef.attrib))
        #             else:
        #                 sectiondef.attrib['name'] = None
        #                 enums_.append(cls(self, **sectiondef.attrib))
        #
        #         elif cls == ENUMVALUE:
        #             if enums_[-1].is_member(sectiondef):
        #                 enums_[-1].add_member(sectiondef)
        #
        #         else:
        #             sectiondef.attrib['name'] = sectiondef[0].text.strip()
        #             cls(self, **sectiondef.attrib)

    def __str__(self):
        return self.template.format(name=self.name)


class FUNC_ARG(object):

    def __init__(self, name, type):
        self.name = name
        self.type = type
        self.description = None


groups = {}


class GROUP(object):
    template = '''\
.. doxygengroup:: {name}
    :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in groups:
            self.__dict__.update(functions[name].__dict__)
        else:
            functions[name] = self
            self.parent = parent
            self.refid = refid
            self.name = name
            self.description = None

    def __str__(self):
        return self.template.format(name=self.name)



class FUNCTION(object):
    template = '''\
.. doxygenfunction:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in functions:
            self.__dict__.update(functions[name].__dict__)
        else:
            functions[name] = self
            self.parent = parent
            self.refid = refid
            self.name = name
            self.types = set()
            self.restype = None
            self.args = []
            self._deps = None
            self.description = None
            self.res_description = None
            self.file_name = None
            self.line_no = None
            self.void_return = False

        if parent is not None:
            root = load_xml(parent.refid)

            for compounddef in root:
                if compounddef.attrib['id'] != parent.refid:
                    continue

                for child in compounddef:
                    if child.tag != 'sectiondef':
                        continue

                    if child.attrib['kind'] != 'func':
                        continue

                    for memberdef in child:
                        if 'id' not in memberdef.attrib:
                            continue

                        if memberdef.attrib['id'] == refid:
                            break
                    else:
                        continue

                    break
                else:
                    continue

                break
            else:
                return

            self.restype = get_type(memberdef)

            for child in memberdef:
                if child.tag == 'type':
                    if child.text and child.text.strip() == 'void':
                        self.void_return = True

                if child.tag == 'param':
                    t = get_type(child)
                    if t is not None:
                        self.types.add(t)

                    for element in child:
                        if element.tag == 'declname':
                            arg = FUNC_ARG(element.text, t)
                            self.args.append(arg)

            for child in memberdef:
                if child.tag == 'location':
                    self.file_name = child.attrib['file']
                    self.line_no = child.attrib['line']

                elif child.tag == 'detaileddescription':
                    self.description = build_docstring(child)
                    for element in child:
                        if element.tag != 'para':
                            continue

                        for desc_element in element:
                            if desc_element.tag == 'simplesect' and desc_element.attrib['kind'] == 'return':
                                self.res_description = build_docstring(desc_element)

                            if desc_element.tag != 'parameterlist':
                                continue

                            for parameter_item in desc_element:
                                parameternamelist = parameter_item[0]
                                if parameternamelist.tag != 'parameternamelist':
                                    continue

                                parameter_name = parameternamelist[0].text

                                try:
                                    parameterdescription = parameter_item[1]
                                    if parameterdescription.tag == 'parameterdescription':
                                        parameter_description = build_docstring(parameterdescription)
                                    else:
                                        parameter_description = None
                                except IndexError:
                                    parameter_description = None

                                if parameter_name is not None:
                                    for arg in self.args:
                                        if arg.name != parameter_name:
                                            continue

                                        arg.description = parameter_description
                                        break
                                    else:
                                        warn(MISSING_FUNC_ARG_MISMATCH, self.name)
                                        warn(None, 'ARG:', parameter_name)
                                        warn(None, 'FILE:', self.file_name)
                                        warn(None, 'LINE:', self.line_no)
                                        warn(None)

            if not self.description:
                warn(MISSING_FUNC, self.name)
                warn(None, 'FILE:', self.file_name)
                warn(None, 'LINE:', self.line_no)
                warn(None)
            else:
                for arg in self.args:
                    if not arg.description:
                        warn(MISSING_FUNC_ARG, self.name)
                        warn(None, 'ARG:', arg.name)
                        warn(None, 'FILE:', self.file_name)
                        warn(None, 'LINE:', self.line_no)
                        warn(None)

                if not self.res_description and not self.void_return:
                    warn(MISSING_FUNC_RETURN, self.name)
                    warn(None, 'FILE:', self.file_name)
                    warn(None, 'LINE:', self.line_no)
                    warn(None)

        if self.restype in self.types:
            self.restype = None

    @property
    def deps(self):
        if self._deps is None:
            self._deps = dict(
                typedefs=set(),
                functions=set(),
                enums=set(),
                structures=set(),
                unions=set(),
                namespaces=set(),
                variables=set(),
            )
            if self.restype is not None:
                self.types.add(self.restype)

            for type_ in self.types:
                if type_ in typedefs:
                    self._deps['typedefs'].add(typedefs[type_])
                elif type_ in structures:
                    self._deps['structures'].add(structures[type_])
                elif type_ in unions:
                    self._deps['unions'].add(unions[type_])
                elif type_ in enums:
                    self._deps['enums'].add(enums[type_])
                elif type_ in functions:
                    self._deps['functions'].add(functions[type_])
                elif type_ in variables:
                    self._deps['variables'].add(variables[type_])
                elif type_ in namespaces:
                    self._deps['namespaces'].add(namespaces[type_])
        return self._deps

    def __str__(self):
        return self.template.format(name=self.name)


class FILE(object):

    def __init__(self, _, refid, name, node, **__):
        if name in files:
            self.__dict__.update(files[name].__dict__)
            return

        files[name] = self

        self.refid = refid
        self.name = name
        self.header_file = os.path.splitext(name)[0]

        enums_ = []

        for member in node:
            if member.tag != 'member':
                continue

            cls = globals()[member.attrib['kind'].upper()]
            if cls == ENUM:
                if member[0].text:
                    member.attrib['name'] = member[0].text.strip()
                    enums_.append(cls(self, **member.attrib))
                else:
                    member.attrib['name'] = None
                    enums_.append(cls(self, **member.attrib))

            elif cls == ENUMVALUE:
                if enums_[-1].is_member(member):
                    enums_[-1].add_member(member)

            else:
                member.attrib['name'] = member[0].text.strip()
                cls(self, **member.attrib)


class ENUM(object):
    template = '''\
.. doxygenenum:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in enums:
            self.__dict__.update(enums[name].__dict__)
        else:

            enums[name] = self

            self.parent = parent
            self.refid = refid
            self.name = name
            self.members = []
            self.description = None
            self.file_name = None
            self.line_no = None

        if parent is not None:
            root = load_xml(parent.refid)

            for compounddef in root:
                if compounddef.attrib['id'] != parent.refid:
                    continue

                for child in compounddef:
                    if child.tag != 'sectiondef':
                        continue

                    if child.attrib['kind'] != 'enum':
                        continue

                    for memberdef in child:
                        if 'id' not in memberdef.attrib:
                            continue

                        if memberdef.attrib['id'] == refid:
                            break
                    else:
                        continue

                    break
                else:
                    continue

                break
            else:
                return
                # raise RuntimeError(f'not able to locate enum {name} ({refid})')

            for element in memberdef:
                if element.tag == 'location':
                    self.file_name = element.attrib['file']
                    self.line_no = element.attrib['line']

                if element.tag == 'detaileddescription':
                    self.description = build_docstring(element)
                elif element.tag == 'enumvalue':
                    item_name = None
                    item_description = None
                    item_file_name = None
                    item_line_no = None

                    for s_element in element:
                        if s_element.tag == 'name':
                            item_name = s_element.text
                        elif s_element.tag == 'detaileddescription':
                            item_description = build_docstring(s_element)

                        elif s_element.tag == 'location':
                            item_file_name = child.attrib['file']
                            item_line_no = child.attrib['line']

                    if item_name is not None:
                        for ev in self.members:
                            if ev.name != item_name:
                                continue
                            break
                        else:
                            ev = ENUMVALUE(
                                self,
                                element.attrib['id'],
                                item_name
                            )

                            self.members.append(ev)

                        ev.description = item_description

            if not self.description:
                warn(MISSING_ENUM, self.name)
                warn(None, 'FILE:', self.file_name)
                warn(None, 'LINE:', self.line_no)
                warn(None)

            for member in self.members:
                if not member.description:
                    warn(MISSING_ENUM_ITEM, self.name)
                    warn(None, 'MEMBER:', member.name)
                    warn(None, 'FILE:', self.file_name)
                    warn(None, 'LINE:', self.line_no)
                    warn(None)

    def is_member(self, member):
        return (
            member.attrib['kind'] == 'enumvalue' and
            member.attrib['refid'].startswith(self.refid)
        )

    def add_member(self, member):
        name = member[0].text.strip()
        for ev in self.members:
            if ev.name == name:
                return

        self.members.append(
            ENUMVALUE(
                self,
                member.attrib['refid'],
                name
            )
        )

    def __str__(self):
        template = [self.template.format(name=self.name)]
        template.extend(list(str(member) for member in self.members))

        return '\n'.join(template)


defines = {}


def build_define(element):
    define = None

    if element.text:
        define = element.text.strip()

    for item in element:
        ds = build_define(item)
        if ds:
            if define:
                define += ' ' + ds
            else:
                define = ds.strip()

    if element.tail:
        if define:
            define += ' ' + element.tail.strip()
        else:
            define = element.tail.strip()

    return define


class DEFINE(object):
    template = '''\
.. doxygendefine:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in defines:
            self.__dict__.update(defines[name].__dict__)
        else:
            defines[name] = self

            self.parent = parent
            self.refid = refid
            self.name = name
            self.description = None
            self.file_name = None
            self.line_no = None
            self.params = None
            self.initializer = None

        if parent is not None:
            root = load_xml(parent.refid)

            for compounddef in root:
                if compounddef.attrib['id'] != parent.refid:
                    continue

                for child in compounddef:
                    if child.tag != 'sectiondef':
                        continue

                    if child.attrib['kind'] != 'define':
                        continue

                    for memberdef in child:
                        if memberdef.attrib['id'] == refid:
                            break
                    else:
                        continue

                    break
                else:
                    continue

                break
            else:
                return

            for element in memberdef:
                if element.tag == 'location':
                    self.file_name = element.attrib['file']
                    self.line_no = element.attrib['line']

                elif element.tag == 'detaileddescription':
                    self.description = build_docstring(element)

                elif element.tag == 'param':
                    for child in element:
                        if child.tag == 'defname':
                            if self.params is None:
                                self.params = []

                            if child.text:
                                self.params.append(child.text)

                elif element.tag == 'initializer':
                    initializer = build_define(element)
                    if initializer is None:
                        self.initializer = ''
                    else:
                        self.initializer = initializer

            if not self.description:
                warn(MISSING_MACRO, self.name)
                warn(None, 'FILE:', self.file_name)
                warn(None, 'LINE:', self.line_no)
                warn(None)

    def __str__(self):
        return self.template.format(name=self.name)


class ENUMVALUE(object):
    template = '''\
.. doxygenenumvalue:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        self.parent = parent
        self.refid = refid
        self.name = name
        self.description = None
        self.file_name = None
        self.line_no = None

    def __str__(self):
        return self.template.format(name=self.name)


class TYPEDEF(object):
    template = '''\
.. doxygentypedef:: {name}
   :project: lvgl
'''

    def __init__(self, parent, refid, name, **_):
        if name in typedefs:
            self.__dict__.update(typedefs[name].__dict__)
        else:
            typedefs[name] = self

            self.parent = parent
            self.refid = refid
            self.name = name
            self.type = None
            self._deps = None
            self.description = None
            self.file_name = None
            self.line_no = None

        if parent is not None:
            root = load_xml(parent.refid)

            for compounddef in root:
                if compounddef.attrib['id'] != parent.refid:
                    continue

                for child in compounddef:
                    if child.tag != 'sectiondef':
                        continue
                    if child.attrib['kind'] != 'typedef':
                        continue

                    for memberdef in child:
                        if 'id' not in memberdef.attrib:
                            continue

                        if memberdef.attrib['id'] == refid:
                            break
                    else:
                        continue

                    break
                else:
                    continue

                break
            else:
                return

            for element in memberdef:
                if element.tag == 'location':
                    self.file_name = element.attrib['file']
                    self.line_no = element.attrib['line']

                if element.tag == 'detaileddescription':
                    self.description = build_docstring(element)

            if not self.description:
                warn(MISSING_TYPEDEF, self.name)
                warn(None, 'FILE:', self.file_name)
                warn(None, 'LINE:', self.line_no)
                warn(None)

            self.type = get_type(memberdef)

    @property
    def deps(self):
        if self._deps is None:
            self._deps = dict(
                typedefs=set(),
                functions=set(),
                enums=set(),
                structures=set(),
                unions=set(),
                namespaces=set(),
                variables=set(),
            )
            if self.type is not None:
                type_ = self.type

                if type_ in typedefs:
                    self._deps['typedefs'].add(typedefs[type_])
                elif type_ in structures:
                    self._deps['structures'].add(structures[type_])
                elif type_ in unions:
                    self._deps['unions'].add(unions[type_])
                elif type_ in enums:
                    self._deps['enums'].add(enums[type_])
                elif type_ in functions:
                    self._deps['functions'].add(functions[type_])
                elif type_ in variables:
                    self._deps['variables'].add(variables[type_])
                elif type_ in namespaces:
                    self._deps['namespaces'].add(namespaces[type_])

        return self._deps

    def __str__(self):
        return self.template.format(name=self.name)


classes = {}


class CLASS(object):

    def __init__(self, _, refid, name, node, **__):
        if name in classes:
            self.__dict__.update(classes[name].__dict__)
            return

        classes[name] = self

        self.refid = refid
        self.name = name

        enums_ = []

        for member in node:
            if member.tag != 'member':
                continue

            cls = globals()[member.attrib['kind'].upper()]
            if cls == ENUM:
                member.attrib['name'] = member[0].text.strip()
                enums_.append(cls(self, **member.attrib))
            elif cls == ENUMVALUE:
                if enums_[-1].is_member(member):
                    enums_[-1].add_member(member)

            else:
                member.attrib['name'] = member[0].text.strip()
                cls(self, **member.attrib)


lvgl_src_path = ''
api_path = ''
html_files = {}


def iter_src(n, p):
    if p:
        out_path = os.path.join(api_path, p)
    else:
        out_path = api_path

    index_file = None

    if p:
        src_path = os.path.join(lvgl_src_path, p)
    else:
        src_path = lvgl_src_path

    folders = []

    for file in os.listdir(src_path):
        if 'private' in file:
            continue

        if os.path.isdir(os.path.join(src_path, file)):
            folders.append((file, os.path.join(p, file)))
            continue

        if not file.endswith('.h'):
            continue

        if not os.path.exists(out_path):
            os.makedirs(out_path)

        if index_file is None:
            index_file = open(os.path.join(out_path, 'index.rst'), 'w')
            if n:
                index_file.write('=' * len(n))
                index_file.write('\n' + n + '\n')
                index_file.write('=' * len(n))
                index_file.write('\n\n\n')

            index_file.write('.. toctree::\n    :maxdepth: 2\n\n')

        name = os.path.splitext(file)[0]
        index_file.write('    ' + name + '\n')

        rst_file = os.path.join(out_path, name + '.rst')
        html_file = os.path.join(p, name + '.html')
        html_files[name] = html_file

        with open(rst_file, 'w') as f:
            f.write('.. _{0}_h:'.format(name))
            f.write('\n\n')
            f.write('=' * len(file))
            f.write('\n')
            f.write(file)
            f.write('\n')
            f.write('=' * len(file))
            f.write('\n\n\n')

            f.write('.. doxygenfile:: ' + file)
            f.write('\n')
            f.write('    :project: lvgl')
            f.write('\n\n')

    for name, folder in folders:
        if iter_src(name, folder):
            if index_file is None:
                index_file = open(os.path.join(out_path, 'index.rst'), 'w')

                if n:
                    index_file.write('=' * len(n))
                    index_file.write('\n' + n + '\n')
                    index_file.write('=' * len(n))
                    index_file.write('\n\n\n')

                index_file.write('.. toctree::\n    :maxdepth: 2\n\n')

            index_file.write('    ' + os.path.split(folder)[-1] + '/index\n')

    if index_file is not None:
        index_file.write('\n')
        index_file.close()
        return True

    return False


def clean_name(nme):
    if nme.startswith('_lv_'):
        nme = nme[4:]
    elif nme.startswith('lv_'):
        nme = nme[3:]

    if nme.endswith('_t'):
        nme = nme[:-2]

    return nme


def is_name_match(item_name, obj_name):
    u_num = item_name.count('_') + 1

    obj_name = obj_name.split('_')
    if len(obj_name) < u_num:
        return False
    obj_name = '_'.join(obj_name[:u_num])

    return item_name == obj_name


def get_includes(name1, name2, obj, includes):
    name2 = clean_name(name2)

    if not is_name_match(name1, name2):
        return

    if obj.parent is not None and hasattr(obj.parent, 'header_file'):
        header_file = obj.parent.header_file
    elif hasattr(obj, 'header_file'):
        header_file = obj.header_file
    else:
        return

    if not header_file:
        return

    if header_file not in html_files:
        return

    includes.add((header_file, html_files[header_file]))


class XMLSearch(object):

    def __init__(self, temp_directory):
        global xml_path
        import subprocess
        import re
        import sys

        bp = os.path.abspath(os.path.dirname(__file__))

        lvgl_path = os.path.join(temp_directory, 'lvgl')
        src_path = os.path.join(lvgl_path, 'src')

        doxy_path = os.path.join(bp, 'Doxyfile')

        with open(doxy_path, 'rb') as f:
            data = f.read().decode('utf-8')

        data = data.replace(
            '#*#*LV_CONF_PATH*#*#',
            os.path.join(temp_directory, 'lv_conf.h')
        )
        data = data.replace('*#*#SRC#*#*', '"{0}"'.format(src_path))

        with open(os.path.join(temp_directory, 'Doxyfile'), 'wb') as f:
            f.write(data.encode('utf-8'))

        status, br = subprocess.getstatusoutput("git branch")
        _, gitcommit = subprocess.getstatusoutput("git rev-parse HEAD")
        br = re.sub('\* ', '', br)

        urlpath = re.sub('release/', '', br)

        os.environ['LVGL_URLPATH'] = urlpath
        os.environ['LVGL_GITCOMMIT'] = gitcommit

        p = subprocess.Popen(
            f'cd "{temp_directory}" && doxygen Doxyfile',
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            shell=True
        )

        out, err = p.communicate()
        if p.returncode:
            if out:
                sys.stdout.write(out)
                sys.stdout.flush()
            if err:
                sys.stderr.write(err)
                sys.stdout.flush()

            sys.exit(p.returncode)

        xml_path = os.path.join(temp_directory, 'xml')

        self.index = load_xml('index')

        for compound in self.index:
            compound.attrib['name'] = compound[0].text.strip()
            if compound.attrib['kind'] in ('example', 'page', 'dir'):
                continue

            globals()[compound.attrib['kind'].upper()](
                None,
                node=compound,
                **compound.attrib
            )

    def get_macros(self):
        return list(defines.values())

    def get_enum_item(self, e_name):
        for enum, obj in enums.items():
            for enum_item in obj.members:
                if enum_item.name == e_name:
                    return enum_item

    def get_enum(self, e_name):
        return enums.get(e_name, None)

    def get_function(self, f_name):
        return functions.get(f_name, None)

    def get_variable(self, v_name):
        return variables.get(v_name, None)

    def get_union(self, u_name):
        return unions.get(u_name, None)

    def get_structure(self, s_name):
        return structures.get(s_name, None)

    def get_typedef(self, t_name):
        return typedefs.get(t_name, None)

    def get_macro(self, m_name):
        return defines.get(m_name, None)


def run(project_path, temp_directory, *doc_paths):
    global base_path
    global xml_path
    global lvgl_src_path
    global api_path

    base_path = temp_directory
    xml_path = os.path.join(base_path, 'xml')
    api_path = os.path.join(base_path, 'API')
    lvgl_src_path = os.path.join(project_path, 'src')

    if not os.path.exists(api_path):
        os.makedirs(api_path)

    iter_src('API', '')
    index = load_xml('index')

    for compound in index:
        compound.attrib['name'] = compound[0].text.strip()
        if compound.attrib['kind'] in ('example', 'page', 'dir'):
            continue

        globals()[compound.attrib['kind'].upper()](
            None,
            node=compound,
            **compound.attrib
        )

    for folder in doc_paths:
        items = list(
            (os.path.splitext(item)[0], os.path.join(folder, item))
            for item in os.listdir(folder)
            if item.endswith('rst') and 'index' not in item
        )

        for name, path in items:
            html_includes = set()

            for container in (
                defines,
                enums,
                variables,
                namespaces,
                structures,
                unions,
                typedefs,
                functions
            ):
                for n, o in container.items():
                    get_includes(name, n, o, html_includes)

            if html_includes:
                html_includes = list(
                    ':ref:`{0}_h`\n'.format(inc)
                    for inc, _ in html_includes
                )

                output = ('\n'.join(html_includes)) + '\n'

                with open(path, 'rb') as f:
                    try:
                        data = f.read().decode('utf-8')
                    except UnicodeDecodeError:
                        print(path)
                        raise

                data = data.split('.. Autogenerated', 1)[0]

                data += '.. Autogenerated\n\n'
                data += output

                with open(path, 'wb') as f:
                    f.write(data.encode('utf-8'))
