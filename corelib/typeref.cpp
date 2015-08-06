// Copyright (C) 2015 Cherokees of Idaho.
//
// This file is part of GNU uCommon C++.
//
// GNU uCommon C++ is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// GNU uCommon C++ is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GNU uCommon C++.  If not, see <http://www.gnu.org/licenses/>.

#include <ucommon-config.h>
#include <ucommon/export.h>
#include <ucommon/typeref.h>
#include <ucommon/string.h>
#include <cstdlib>

namespace ucommon {

TypeRef::Counted::Counted(void *addr, size_t objsize) : 
ObjectProtocol()
{
    this->memory = addr;
    this->size = objsize;
}

void TypeRef::Counted::dealloc()
{
    delete this;
    ::free(memory);
}

void TypeRef::Counted::operator delete(void *addr)
{
}

void TypeRef::Counted::retain(void)
{
    count.fetch_add();
}

void TypeRef::Counted::release(void)
{
    if(count.fetch_sub() < 2) {
	    dealloc();
    }
}

TypeRef::TypeRef()
{
    ref = NULL;
}

TypeRef::TypeRef(TypeRef::Counted *object)
{
    ref = object;
    if(ref)
        ref->retain();
}

TypeRef::TypeRef(const TypeRef& copy)
{
    ref = copy.ref;
    if(ref)
        ref->retain();
}

TypeRef::~TypeRef()
{
    release();
}

void TypeRef::release(void)
{
    if(ref)
    	ref->release();
    ref = NULL;
}

void TypeRef::set(const TypeRef& ptr)
{
    if(ptr.ref)
    	ptr.ref->retain();
    release();
    ref = ptr.ref;
}

void TypeRef::set(TypeRef::Counted *object)
{
    if(object)
        object->retain();
    release();
    ref = object;
}

caddr_t TypeRef::alloc(size_t size)
{
    return (caddr_t)::malloc(size + 16);
}

caddr_t TypeRef::mem(caddr_t addr)
{
    while(((uintptr_t)addr) & 0xf)
        ++addr;
    return addr;
}

stringref::value::value(caddr_t addr, size_t objsize, const char *str) : 
TypeRef::Counted(addr, objsize)
{
    if(str)
    	String::set(mem, objsize + 1, str);
    else
	    mem[0] = 0;
}

void stringref::value::destroy(void) 
{
	count.clear();
	release();
}

stringref::stringref() :
TypeRef() {}

stringref::stringref(const stringref& copy) :
TypeRef(copy) {}

stringref::stringref(const char *str) :
TypeRef()
{
    size_t size = 0;
    
    if(str)
        size = strlen(str);

    caddr_t p = TypeRef::alloc(sizeof(value) + size);
    TypeRef::set(new(mem(p)) value(p, size, str));
}

const char *stringref::operator()(ssize_t offset) const
{
    value *v = polystatic_cast<value *>(ref);
    if(!v)
        return NULL;

    if(offset < 0 && offset < -((ssize_t)v->len()))
        return NULL;

    if(offset < 0)
        return &v->mem[v->len() + offset];

    if(offset > (ssize_t)v->len())
        return NULL;

    return &v->mem[v->len() + offset];
}

const char *stringref::operator*() const 
{
    if(!ref)
	    return NULL;
    value *v = polystatic_cast<value *>(ref);
    return &v->mem[0];
}

stringref& stringref::operator=(const stringref& objref)
{
    TypeRef::set(objref);
    return *this;
}

stringref& stringref::operator=(const char *str)
{
    set(str);
    return *this;
}

void stringref::set(const char *str)
{
    release();
    size_t size = 0;

    if(str)
        size = strlen(str);

    caddr_t p = TypeRef::alloc(sizeof(value) + size);
    TypeRef::set(new(mem(p)) value(p, size, str));
}

void stringref::assign(value *chars)
{
    release();
    chars->size = strlen(chars->mem);
    TypeRef::set(chars);
}

stringref& stringref::operator=(value *chars)
{
    assign(chars);
    return *this;
}

stringref::value *stringref::create(size_t size)
{
    caddr_t p = TypeRef::alloc(sizeof(value) + size);
    return new(mem(p)) value(p, size, NULL);
}

void stringref::destroy(stringref::value *chars)
{
    if(chars)
        chars->destroy();
}

void stringref::expand(stringref::value **handle, size_t size)
{
    if(!handle || !*handle)
        return;

    stringref::value *change = create(size + (*handle)->max());
    if(change)
        String::set(change->get(), change->max() + 1, (*handle)->get());
    destroy(*handle);
    *handle = change;
}

byteref::value::value(caddr_t addr, size_t objsize, const uint8_t *str) : 
TypeRef::Counted(addr, objsize)
{
    if(objsize)
        memcpy(mem, str, objsize);
}

byteref::value::value(caddr_t addr, size_t size) : 
TypeRef::Counted(addr, size)
{
}

byteref::byteref() :
TypeRef() {}

byteref::byteref(const byteref& copy) :
TypeRef(copy) {}

byteref::byteref(const uint8_t *str, size_t size) :
TypeRef()
{
    caddr_t p = TypeRef::alloc(sizeof(value) + size);
    TypeRef::set(new(mem(p)) value(p, size, str));
}

void byteref::value::destroy(void) 
{
	count.clear();
	release();
}

const uint8_t *byteref::operator*() const 
{
    if(!ref)
	    return NULL;
    value *v = polystatic_cast<value *>(ref);
    return &v->mem[0];
}

byteref& byteref::operator=(const byteref& objref)
{
    TypeRef::set(objref);
    return *this;
}

byteref& byteref::operator=(value *bytes)
{
    assign(bytes);
    return *this;
}

void byteref::set(const uint8_t *str, size_t size)
{
    release();
    caddr_t p = TypeRef::alloc(sizeof(value) + size);
    TypeRef::set(new(mem(p)) value(p, size, str));
}

void byteref::assign(value *bytes)
{
    release();
    TypeRef::set(bytes);
}

byteref::value *byteref::create(size_t size)
{
    caddr_t p = TypeRef::alloc(sizeof(value) + size);
    return new(mem(p)) value(p, size);
}

void byteref::destroy(byteref::value *bytes)
{
    if(bytes)
        bytes->destroy();
}

TypeRef::Counted *ArrayRef::Array::get(size_t index)
{
    if(index >= size)
        return NULL;

    return (get())[index];
}

size_t TypeRef::size(void) const
{
    if(!ref)
        return 0;

    return ref->size;
}

unsigned TypeRef::copies() const 
{
	if(!ref)
		return 0;
	return ref->copies();
}

ArrayRef::Array::Array(void *addr, size_t size) :
Counted(addr, size)
{
    size_t index = 0;
    Counted **list = get();

    if(!size)
        return;

    while(index < size) {
        list[index++] = NULL;
    }
}

void ArrayRef::Array::dealloc()
{
    size_t index = 0;
    Counted **list = get();

    if(!size)
        return;

    while(index < size) {
        Counted *object = list[index];
        if(object) {
            object->release();
            list[index] = NULL;
        }
        ++index;
    }
    size = 0;
    Counted::dealloc();
}

void ArrayRef::Array::assign(size_t index, Counted *object)
{
    if(index >= size)
        return;
    
    if(object)
        object->retain();

    Counted *replace = get(index);
    if(replace)
        replace->release();

    (get())[index] = object;
}   

ArrayRef::ArrayRef() :
TypeRef()
{
}

ArrayRef::ArrayRef(size_t size) :
TypeRef(create(size))
{
} 

ArrayRef::ArrayRef(const ArrayRef& copy) :
TypeRef(copy)
{
}

void ArrayRef::reset(TypeRef& var)
{
    size_t index = 0;
    Array *array = polystatic_cast<Array *>(ref);
    Counted *object = var.ref;

    if(!array || !array->size || !object)
        return;

    while(index < array->size) {
        array->assign(index++, object);
    }
}

ArrayRef::Array *ArrayRef::create(size_t size)
{
    if(!size)
        return NULL;

    size_t s = sizeof(Array) + (size * sizeof(Counted *));
    caddr_t p = TypeRef::alloc(s);
    return new(mem(p)) Array(p, size);
}

void ArrayRef::assign(size_t index, TypeRef& t)
{
    Array *array = polystatic_cast<Array *>(ref);
    if(!array || index >= array->size)
        return;

    Counted *obj = t.ref;
    array->assign(index, obj);
}

void ArrayRef::resize(size_t size)
{
    Array *array = create(size);
    Array *current = polystatic_cast<Array *>(ref);
    size_t index = 0;

    if(array && current) {
        while(index < size && index < current->size) {
            array->assign(index, current->get(index));
            ++index;
        }
    }
    TypeRef::set(array);
}

TypeRef::Counted *ArrayRef::get(size_t index)
{
    Array *array = polystatic_cast<Array*>(ref);
    if(!array)
        return NULL;

    return array->get(index);
}

} // namespace
