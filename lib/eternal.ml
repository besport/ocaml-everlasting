
type 'a t
      
external create : int -> 'a t = "ocaml_create"
external t1 : 'a -> 'a ref = "ocaml_t1" 
external set : 'a t -> int -> 'a -> unit = "ocaml_set"  
external unsafe_get : 'a t -> int -> 'a  = "ocaml_unsafe_get"
external copy : 'a -> 'a = "ocaml_copy" 
    
