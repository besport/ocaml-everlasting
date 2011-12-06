/*
 * 12/4/2011 - eternal
 */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/fail.h>


// type definition 
struct eternal 
{
  void *data ;

}; 

// Handle the eternal struct
#define Eternal_val(v) (*((struct eternal **) Data_custom_val(v))) 

void finalize (value v) {
  struct eternal *t = Eternal_val(v) ;
  printf ("> finalize me!\n"); 
  if (t->data) free (t->data); 
  return ;
}
 
static struct custom_operations context = {
  "eternal.t",
  //(void *)finalize,
  finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default
};

value alloc_eternal (struct eternal *t) {
  value v = alloc_custom(&context, sizeof(struct eternal*), 0, 1);
  Eternal_val (v) = t;
  return v;
};

// Create the context 

value ocaml_create_eternal (value unit) {
  CAMLparam1(unit);

  struct eternal *t = (struct eternal *)malloc ((sizeof (struct eternal))) ; 
  t->data = 0 ; 
  CAMLreturn (alloc_eternal (t)); 
}

/*

// Perform the lookup 

static const char * _mk_NA( const char * p ){
 return p ? p : "N/A";
}

value ocaml_GeoIP_record_by_addr (value context, value address){
  CAMLparam2(context, address); 
  CAMLlocal1(result) ; 
  
  GeoIPRecord *gir = GeoIP_record_by_addr (Context_val(context), String_val (address)) ;

  if (gir == NULL) caml_failwith("Can't initialize gir") ;
  
  result = caml_alloc (6 ,0);

  Store_field(result, 0, (caml_copy_string(_mk_NA (gir->country_code)))); 
  Store_field(result, 1, (caml_copy_string(_mk_NA (gir->country_code3))));
  Store_field(result, 2, (caml_copy_string(_mk_NA (gir->country_name))));
  Store_field(result, 3, (caml_copy_string(_mk_NA (gir->city))));
  Store_field(result, 4, (caml_copy_double(gir->latitude)));
  Store_field(result, 5, (caml_copy_double(gir->longitude)));
  

     Store_field(result, 5, (caml_copy_string(gir->postal_code))); 
     Store_field(result, 7, (caml_copy_int32(gir->metro_code)));
     Store_field(result, 8, (caml_copy_int32(gir->dma_code)));
     Store_field(result, 9, (caml_copy_int32(gir->area_code)));
     Store_field(result, 10, (caml_copy_int32(gir->charset)));
     Store_field(result, 11, (caml_copy_string(gir->continent_code)));


  GeoIPRecord_delete(gir);
  CAMLreturn (result) ;
}


*/