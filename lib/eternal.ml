
type 'a t
      
external create : int -> 'a t = "ocaml_create"

external set : 'a t -> int -> 'a -> unit = "ocaml_set"  
external get : 'a t -> int -> 'a  = "ocaml_get"

(** copy an ocaml element outside of the heap, to use with some care .. *)
external copy : 'a -> 'a = "ocaml_copy" 
    
   
