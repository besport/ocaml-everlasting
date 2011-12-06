
type 'a t
      
external create : int -> 'a t = "ocaml_create"
external set : 'a t -> int -> 'a -> unit = "ocaml_set"  
external unsafe_get : 'a t -> int -> 'a  = "ocaml_unsafe_get"

    
