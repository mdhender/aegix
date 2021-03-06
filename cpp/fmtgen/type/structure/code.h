//
// aegis - project change supervisor
// Copyright (C) 2008 Peter Miller
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>.
//

#ifndef FMTGEN_TYPE_STRUCTURE_CODE_H
#define FMTGEN_TYPE_STRUCTURE_CODE_H

#include <fmtgen/type/structure.h>

/**
  * The type_structure_code class is used to represent the code
  * generation necessary for a structure type, in the original (circa
  * 1991) C style.
  */
class type_structure_code:
    public type_structure
{
public:
    /**
      * The destructor.
      */
    virtual ~type_structure_code();

private:
    /**
      * The constructor.
      * It is private on purpose, use the #create class method instead.
      *
      * @param gen
      *     where to write the generated code
      * @param name
      *     The name of this structure type.
      * @param global
      *     true if the name is a global name, false if it is local to
      *     the containing .def file.
      */
    type_structure_code(generator *gen, const nstring &name, bool global);

public:
    /**
      * The create class method is used to create new dynamically
      * allocated instances of this class.
      *
      * @param gen
      *     where to write the generated code
      * @param name
      *     The name of this structure type.
      * @param global
      *     true if the name is a global name, false if it is local to
      *     the containing .def file.
      */
    static pointer create(generator *gen, const nstring &name, bool global);

protected:
    // See base class for documentation.
    void gen_body() const;

    // See base class for documentation.
    void gen_declarator(const nstring &variable_name, bool is_a_list,
        int attributes, const nstring &comment) const;

    // See base class for documentation.
    void gen_call_xml(const nstring &form_name, const nstring &member_name,
        int attributes) const;

    // See base class for documentation.
    void gen_copy(const nstring &member_name) const;

    // See base class for documentation.
    void gen_trace(const nstring &name, const nstring &value) const;

    // See base class for documentation.
    void gen_free_declarator(const nstring &variable_name, bool is_a_list)
        const;

private:
    /**
      * The default constructor.  Do not use.
      */
    type_structure_code();

    /**
      * The copy constructor.  Do not use.
      */
    type_structure_code(const type_structure_code &);

    /**
      * The assignment operator.  Do not use.
      */
    type_structure_code &operator=(const type_structure_code &);
};

#endif // FMTGEN_TYPE_STRUCTURE_CODE_H
