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



#ifndef CAML_GC_H
#define CAML_GC_H


#define Caml_white (0 << 8)
#define Caml_gray  (1 << 8)
#define Caml_blue  (2 << 8)
#define Caml_black (3 << 8)

#define Color_hd(hd) ((color_t) ((hd) & Caml_black))
#define Color_hp(hp) (Color_hd (Hd_hp (hp)))
#define Color_val(val) (Color_hd (Hd_val (val)))

#define Is_white_hd(hd) (Color_hd (hd) == Caml_white)
#define Is_gray_hd(hd) (Color_hd (hd) == Caml_gray)
#define Is_blue_hd(hd) (Color_hd (hd) == Caml_blue)
#define Is_black_hd(hd) (Color_hd (hd) == Caml_black)

/* This depends on the layout of the header.  See [mlvalues.h]. */
#define Make_header(wosize, tag, color)                                       \
      (/*Assert ((wosize) <= Max_wosize),*/                                   \
       ((header_t) (((header_t) (wosize) << 10)                               \
                    + (color)                                                 \
                    + (tag_t) (tag)))                                         \
      )

#endif

// type definition 
struct eternal 
{
  value **data ;

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

value ocaml_create (value size) {
  CAMLparam1(size);

  struct eternal *t = (struct eternal *)malloc ((sizeof (struct eternal))) ; 
  t->data = (value **)malloc (sizeof (value **) * Int_val(size)) ; 
  CAMLreturn (alloc_eternal (t)); 
}

// create eternal references 

value ocaml_t1 (value v) {
  CAMLparam1 (v); 
  CAMLlocal1 (b) ; 
  b = caml_alloc (1, 0) ;
  caml_register_global_root(&b) ;

  printf ("--> tag white %d gray %d  blue %d   black %d  %d\n", Caml_white, Caml_gray, Caml_blue, Caml_black, Color_val(v));  
   
  Store_field (b, 0, v); 
  CAMLreturn (b) ;
}

// Get and set

value ocaml_unsafe_get (value e, value pos) {
  CAMLparam2(e, pos); 
  CAMLreturn(*(Eternal_val(e)->data[Int_val (pos)])) ; 
}


void ocaml_set (value e, value pos, value v) {
  CAMLparam2(e, pos); 
  
  int p = Int_val (pos) ;
  
  printf ("--> tag white %d gray %d  blue %d   black %d  %d\n", Caml_white, Caml_gray, Caml_blue, Caml_black, Color_val(v));  
  
  struct eternal *t = Eternal_val (e) ; 
  
  (t->data) [ p ] = &v ; /* this does not work as gc relocate the values */

  CAMLreturn0 ;
}




/* Copy OCaml blocks outside of the heap */


#define Alloc_small_nogc(result,wosize,tag)                             \
          void *ptr = (void *) malloc (Bhsize_wosize (wosize) * sizeof (int)) ; \
          Hd_hp (ptr) = Make_header (wosize, tag, Caml_black) ;\
          result = Val_hp (ptr) ;

value ocaml_copy (value e) {
  
  // CAMLparam1(e) ;
  if Is_block (e) 
    {
      // Handling block
      //printf ("it's a block\n") ;
      switch (Tag_val(e))
        {
        case Closure_tag: { printf ("closure_tag\n"); break;  }
        case String_tag: { 
          // printf ("string_tag\n");
          
          value result ; 
          mlsize_t wosize ; 

          wosize = (caml_string_length (e) + sizeof (value)) / sizeof (value);
          
          Alloc_small_nogc(result, wosize, String_tag)
            
          memmove (String_val(result), String_val (e), caml_string_length (e)) ;  
          return result ;
          
        }
        case Double_tag: {
          printf ("double_tag\n");
          value result ; 
          Alloc_small_nogc(result, Double_wosize, Double_tag); 
          Store_double_val (result, Double_val (e)); 
          return result ; 
        }
        case Double_array_tag: 
          {
            printf ("double array tag\n");
            value result ;
            Alloc_small_nogc(result, Wosize_val (e), Double_array_tag); 
            int i; 
            for (i=0; i < Wosize_val (e); i++)
              {
                Store_double_field(result, i, Double_field(e, i)); 
              }; 
            return result; 
          }
        case Abstract_tag: { printf ("closure_tag\n"); break;  }
        case Custom_tag: { printf ("closure_tag\n"); break;  }
        default: 
          {
            printf ("> code\n"); 
            value result; 
            Alloc_small_nogc(result, Wosize_val (e), Tag_val (e)) ; 
            int i; 
            for (i=0; i < Wosize_val (e); i++)
              {
                Store_field (result, i, ocaml_copy (Field (e, i))); 
              }; 
              
            return result ;
          }
        }
      
    }
  else 
    {
      printf ( "straight\n" ); 
     
      return e ; 
    }
}


/*

      value b = caml_alloc (Wosize_val (e), Tag_val (e)) ; 
      int i; 
      for (i=0; i < Wosize_val (e); i++)
        {
          Store_field (b, i, ocaml_copy (Field (e, i))); 
        }; 
      return b ;

*/
