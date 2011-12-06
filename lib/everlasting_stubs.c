/*
 * 12/4/2011 - eternal
 */


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* Copy OCaml blocks outside of the heap */


#define Alloc_small_nogc(result,wosize,tag)                             \
          void *ptr = (void *) malloc (Bhsize_wosize (wosize) * sizeof (int)) ; \
          Hd_hp (ptr) = Make_header (wosize, tag, Caml_black) ;\
          result = Val_hp (ptr) ;

#define Free_small_nogc(result)\
    free (result - 8 ) ;


value ocaml_copy (value e) {
  
  // CAMLparam1(e) ;
  if Is_block (e) 
    {
      // Handling block
      
      switch (Tag_val(e))
        {
        case Closure_tag: { caml_invalid_argument("Can't copy Closure_tag blocks") ; break; }
        case String_tag: { 
          
          
          mlsize_t len ; 
          value result ; 
          mlsize_t offset_index;
          mlsize_t wosize ; 
          len = caml_string_length (e) ; 
          wosize = (len + sizeof (value)) / sizeof (value);
          
          Alloc_small_nogc(result, wosize, String_tag); 
          Field (result, wosize - 1) = 0;
          offset_index = Bsize_wsize (wosize) - 1;
          Byte (result, offset_index) = offset_index - len;
          memmove (String_val(result), String_val (e), len) ;  
          return result ;
          
        }
        case Double_tag: {
          
          value result ; 
          Alloc_small_nogc(result, Double_wosize, Double_tag); 
          Store_double_val (result, Double_val (e)); 
          return result ; 
        }
        case Double_array_tag: 
          {
            
            value result ;
            Alloc_small_nogc(result, Wosize_val (e), Double_array_tag); 
            int i; 
            for (i=0; i < Wosize_val (e); i++)
              {
                Store_double_field(result, i, Double_field(e, i)); 
              }; 
            return result; 
          }
        case Abstract_tag: { caml_invalid_argument("Can't copy Abstract_tag blocks") ; break;  }
        case Custom_tag: { 
          value result ; 
        
          // Copying custom tags. it might be the tricky part 
          Alloc_small_nogc(result, Wosize_val (e), Custom_tag);
          Custom_ops_val(result) = Custom_ops_val(e); // I imagine that it's a global value, anyway it's useless 
          
          // Copying the custom data 
          
          Int64_val(result) = Int64_val(e) ; 
          
        
          return result;  }
        default: 
          {
            
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
      return e ; 
    }
}


value ocaml_update (value legacy, value latest) {
  if (Is_block (legacy) && Is_block (latest))  {
    if (Tag_val (legacy) == Tag_val (latest)) 
      {
        switch (Tag_val(legacy)) 
          {
          case Closure_tag: { caml_invalid_argument("Can't update Closure_tag blocks") ; break; }
          case Abstract_tag: { caml_invalid_argument("Can't update Abstract_tag blocks") ; break;  }
          case Custom_tag: { 
            
            
            if ((Wosize_val (legacy) == Wosize_val (latest)) && (!(memcmp (Data_custom_val(legacy), Data_custom_val(latest), Wosize_val (legacy)))))
              {

                
                return legacy ;
              }
            else 
              {
                
                value result ; 
        
                // Copying custom tags. it might be the tricky part 
                Alloc_small_nogc(result, Wosize_val (legacy), Custom_tag);
                Custom_ops_val(result) = Custom_ops_val(legacy); // I imagine that it's a global value, anyway it's useless 
                
                // Copying the custom data 
                
                Int64_val(result) = Int64_val(legacy) ; 
                Free_small_nogc (legacy); 
                return result ;
                
              }
            

          }
            
          case String_tag: { 
            
            
            if (!strcmp (String_val (legacy), String_val (latest)))
              return legacy ; 
            else 
              {
                mlsize_t len ; 
                value result ; 
                mlsize_t offset_index;
                mlsize_t wosize ; 
                len = caml_string_length (latest) ; 
                wosize = (len + sizeof (value)) / sizeof (value);
          
                Alloc_small_nogc(result, wosize, String_tag); 
                Field (result, wosize - 1) = 0;
                offset_index = Bsize_wsize (wosize) - 1;
                Byte (result, offset_index) = offset_index - len;
                memmove (String_val(result), String_val (latest), len) ;  
                
                Free_small_nogc(legacy); 
                
                return result ;
                
              }
          }
            
            
          default: 
            {
              if (Wosize_val (legacy) == Wosize_val (latest)) 
                {
                  int i; 
                  for (i=0; i < Wosize_val (legacy); i++)
                    {
                      value updated = ocaml_update (Field(legacy, i), Field(latest, i)) ; 
                      if (updated != Field(legacy, i)) 
                        Store_field (legacy, i, updated); 
                    }; 
                  
                  return legacy ;
                }
              else 
                {
                  caml_invalid_argument("Legacy and latest have different wosizes") ; 
                }
            }
          }
        
      }
    else 
      {
        caml_invalid_argument("Legacy and latest have different tags") ; 
      }
  }
  else if (Is_long (legacy) && Is_long (latest)) {
    return latest ;
  } 
  else {
    if (Is_block (legacy)) Free_small_nogc(legacy) ;
    value result ; 
    result = ocaml_copy (latest); 
    return result ; 
  }
}



// the everlasting lib


// type definition 
struct eternal 
{
  void **data ;
}; 

// Handle the eternal struct

#define Eternal_val(v) (*((struct eternal **) Data_custom_val(v))) 

void finalize (value v) {
  struct eternal *t = Eternal_val(v) ;
  
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
  t->data = (void **)calloc (Int_val(size), sizeof (void *)) ; /* use calloc to zero the memory, it's slower but muuuch safer in our case */
  CAMLreturn (alloc_eternal (t)); 
}

// Get and set

value ocaml_get (value e, value pos) {
  CAMLparam2(e, pos);

  if (!(Eternal_val(e)->data[Int_val (pos)])) { caml_invalid_argument("There is no such element") ; }
  CAMLreturn (Eternal_val(e)->data[Int_val (pos)]) ; 
}


void ocaml_set (value e, value pos, value v) {
  CAMLparam3(e, pos, v); 
 
  int p = Int_val (pos) ;
  
  struct eternal *t = Eternal_val (e) ; 
  
  (t->data)[p] = ocaml_copy (v) ; /* this does not work as gc relocate the values */

  CAMLreturn0 ;
}



void ocaml_replace (value e, value pos, value v){
  CAMLparam3 (e, pos, v); 
  
  
  int p = Int_val (pos) ; 
  struct eternal *t = Eternal_val (e) ; 
 
  if ((t->data)[p])
    {
      (t->data)[p] = ocaml_update ((t->data)[p], v) ;
    } 
  else
    {
      (t->data)[p] = ocaml_copy (v) ; 
    } 

  CAMLreturn0 ; 
}


