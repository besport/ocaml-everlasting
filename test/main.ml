let debug fmt = Printf.ksprintf print_endline fmt

let time f () = 
  let t1 = Unix.gettimeofday () in 
  f () ; 
  let t2 = Unix.gettimeofday () in 
  t2 -. t1 



type permission = User | SuperAdministrator | Administrator

type plist = permission list

type datetime = { month: int; day: int; year: int; hour : int; minute: int; }

type gender = Male | Female | Unknown 


type t = 
    {
      date_joined : datetime ;
    
      name : string ; 
  (*    first_name : string ; 
      middle_name : string ; 
      last_name : string ; *)

      facebook_id : string option ;
(*      facebook_access_token : string option ;

      twitter_id : string option ;     

      statuses : Object_status.t __pointers__ ; 
      sports : Object_sport.t __pointers__ ; 

      tagline : string option ; 
      cover_photo : string option ; 

      birthday : string option ; 
      gender : gender ;

      (* social features *)

      following : t __pointers__ ;


*)      followers : int64 list ; 
(*
      (* permissions, needs to be server side checked of course *)
*)
      permissions: plist ;
(*
      (* set if the user is active or no *)

      ghost: bool ; 
  *)    }   

let rec listn = function 
  | 0 -> []
  | n -> (Int64.of_int n) :: (listn (n-1))

let create () = 
  {
    date_joined = {  month = 1 ; day = 3; year = 4; hour = 7 ; minute = 10; } ; 
    name = "william le ferrand" ;
    facebook_id = Some "coucou" ;
    permissions = [ User ] ;
    followers = listn 100; 
  }



let test6 () = 
  let e = Everlasting.create 100 in
  let _ = 
    let u = (create ()) in
    Everlasting.set e 1 u in 
  Gc.full_major () ;
  let v = Everlasting.get e 1 in 
  debug "> name is %s %d %s %d" v.name v.date_joined.minute (match v.facebook_id with Some s -> s | None -> "__none__")
    (List.length v.followers) ; 
  
  List.iter (Printf.printf "%Ld\n") v.followers

let _ = 
  test6 () ;  
  debug "> collected, exiting" 
    
  
