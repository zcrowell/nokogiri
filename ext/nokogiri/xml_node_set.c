#include <xml_node_set.h>
#include <libxml/xpathInternals.h>
/*
 * call-seq:
 *  length
 *
 * Get the length of the node set
 */
static VALUE length(VALUE self)
{
  xmlNodeSetPtr node_set;
  Data_Get_Struct(self, xmlNodeSet, node_set);

  if(node_set)
    return INT2NUM(node_set->nodeNr);

  return INT2NUM(0);
}

/*
 * call-seq:
 *  push(node)
 *
 * Append +node+ to the NodeSet.
 */
static VALUE push(VALUE self, VALUE rb_node)
{
  xmlNodeSetPtr node_set;
  xmlNodePtr node;

  Data_Get_Struct(self, xmlNodeSet, node_set);
  Data_Get_Struct(rb_node, xmlNode, node);
  xmlXPathNodeSetAdd(node_set, node);
  return self;
}

/*
 * call-seq:
 *  [](i)
 *
 * Get the node at index +i+
 */
static VALUE index_at(VALUE self, VALUE number)
{
  int i = NUM2INT(number);
  xmlNodeSetPtr node_set;
  Data_Get_Struct(self, xmlNodeSet, node_set);

  if(i >= node_set->nodeNr || abs(i) > node_set->nodeNr)
    return Qnil;

  if(i < 0)
    i = i + node_set->nodeNr;

  return Nokogiri_wrap_xml_node(node_set->nodeTab[i]);
}

static void gc_mark(xmlNodeSetPtr node_set)
{
    int j ;
    for (j = 0 ; j < node_set->nodeNr ; ++j) {
        if (node_set->nodeTab[j]->_private)
            rb_gc_mark((VALUE)node_set->nodeTab[j]->_private);
    }
}

static void deallocate(xmlNodeSetPtr node_set)
{
  xmlXPathFreeNodeSet(node_set);
}

static VALUE allocate(VALUE klass)
{
  return Nokogiri_wrap_xml_node_set(xmlXPathNodeSetCreate(NULL));
}

VALUE Nokogiri_wrap_xml_node_set(xmlNodeSetPtr node_set)
{
    return Data_Wrap_Struct(cNokogiriXmlNodeSet, gc_mark, deallocate, node_set);
}

VALUE cNokogiriXmlNodeSet ;
void init_xml_node_set(void)
{
  VALUE klass = cNokogiriXmlNodeSet = rb_eval_string("Nokogiri::XML::NodeSet");
  rb_define_alloc_func(klass, allocate);
  rb_define_method(klass, "length", length, 0);
  rb_define_method(klass, "[]", index_at, 1);
  rb_define_method(klass, "push", push, 1);
}
