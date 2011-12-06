
type 'a t
      
external create : int -> 'a t = "ocaml_create"


(* when calling set, the value shouldn't have been created in the call *)

external set : 'a t -> int -> 'a -> unit = "ocaml_set"  
external get : 'a t -> int -> 'a  = "ocaml_get"
external replace : 'a t -> int -> 'a -> unit = "ocaml_replace"


(** copy an ocaml element outside of the heap, to use with some care .. *)
external copy : 'a -> 'a = "ocaml_copy" 
(** update an element outside of the heap with an element inside the heap *)
external update : 'a -> 'a -> 'a = "ocaml_update"    
   
