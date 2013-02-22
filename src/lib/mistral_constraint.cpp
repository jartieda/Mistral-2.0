
/*
  Mistral 2.0 is a constraint satisfaction and optimisation library
  Copyright (C) 2009  Emmanuel Hebrard
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  The author can be contacted electronically at emmanuel.hebrard@gmail.com.
*/

#include <cmath>

#include <mistral_sat.hpp>
#include <mistral_solver.hpp>
#include <mistral_variable.hpp>
#include <mistral_constraint.hpp>






std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::Constraint& x) {
  return x.display(os);
}

std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::Constraint* x) {
  return x->display(os);
}


std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::ConstraintImplementation& x) {
  return x.display(os);
}

std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::ConstraintImplementation* x) {
  return (x ? x->display(os) : os << "Null");
}


std::ostream& Mistral::operator<< (std::ostream& os,  Mistral::ConstraintTriggerArray& x) {
  return x.display(os);
}

std::ostream& Mistral::operator<< (std::ostream& os,  Mistral::ConstraintTriggerArray* x) {
  return x->display(os);
}



// void Mistral::Constraint::set_rank(const int idx) {
//   int rank = (data^CTYPE);
//   propagator->index[rank] = idx;
// }

// void Mistral::Constraint::set_id(const int idx) {
//   propagator->id = idx;
// }

// int Mistral::Constraint::id() {
//   return propagator->id;
// }


//   void Mistral::Constraint::post(solver*) { propagator->initial_post(solver); }

//   int Mistral::Constraint::check(int* sol) {
//     return propagator->check(sol);
//   }

//   Mistral::PropagationOutcome Mistral::Constraint::propagate() {
//     return propagator->propagate();
//   }

//   Mistral::PropagationOutcome Mistral::Constraint::propagate(const int changed_idx, const Event evt) {
//     return propagator->propagate(changed_idx, evt);
//   }

// void Mistral::Constraint::restore() {}


Mistral::ConstraintTriggerArray::ConstraintTriggerArray() {}

Mistral::ConstraintTriggerArray::ConstraintTriggerArray(const int size) {
  initialise(size);
}

void Mistral::ConstraintTriggerArray::initialise(const int size) {
  for(int i=0; i<3; ++i) on[i].initialise(size);
}

Mistral::ConstraintTriggerArray::~ConstraintTriggerArray() {
#ifdef _DEBUG_MEMORY
  std::cout << "c delete constraint trigger array" << std::endl;
#endif
}

std::ostream& Mistral::ConstraintTriggerArray::display(std::ostream& os) const {
  for(int i=2; i>=0; --i) {
    os << "["; 
    for(unsigned int j=0; j<on[i].size; ++j) {
      os << on[i][j] ; //<< ":"
	//<< on[i][j].index();
      if(j<on[i].size-1) os << ", ";
    }
    os << "]";
  }
  return os;
}


Mistral::ConstraintImplementation::ConstraintImplementation() {
  id = -1;
  //arity = 0;
  //trigger = NULL;
  self = NULL;
  index = NULL;
  enforce_nfc1 = true;
}

// Mistral::ConstraintImplementation::ConstraintImplementation(const int a) {
//   id = -1;
//    //arity = a;
// //   //num_triggers = 0;
// //   //self = new Constraint[arity];
// //   //index = new int[arity];
// //   //trigger = new Trigger*[arity];
// // }

Mistral::ConstraintImplementation::~ConstraintImplementation() {
#ifdef _DEBUG_MEMORY
  std::cout << "c delete constraint implementation" << std::endl;
#endif
  delete [] self;
  delete [] index;
  //delete [] trigger;
}

void Mistral::ConstraintImplementation::trigger_on(const int t, Variable x) {

  // display(std::cout);
  // std::cout << " triggers on " 
  // 	    << (t ? (t>1 ? "domain" : "range") : "value")  << " of " << x << std::endl;

  // //std::cout << "on[" << on.size << "] = ";

  
  //std::cout << x.domain_type << " " << CONST_VAR << std::endl;

  if(x.domain_type != CONST_VAR) {
    //Solver *solver = x.get_solver();

    // std::cout << (*this) << " triggers on "  << t << " "
    // 	    << (int*)(&(solver->constraint_graph[x.id()].on[t])) << std::endl;
    
    
    on.add(&(get_solver()->constraint_graph[x.id()].on[t]));
  } else {
    on.add(NULL);
  }
  _scope.add(x);
  //std::cout << on.back() << std::endl;

}

bool Mistral::ConstraintImplementation::is_triggered_on(const int i, const int t) {

  // std::cout << (*this) << " " << t << std::endl;

  // std::cout << (int*)(on[i]) << " >=? "
  // 	    << (int*)(&(get_solver()->constraint_graph[_scope[i].id()].on[t])) 
  // 	    << (on[i] >= &(get_solver()->constraint_graph[_scope[i].id()].on[t]) ? " yes" : " no") << std::endl;

  


  return( on[i] >= &(get_solver()->constraint_graph[_scope[i].id()].on[t]) );
}


// int Mistral::Constraint::get_backtrack_level() {
//   return solver->level-1;
// }

// Mistral::Decision Mistral::ConstraintImplementation::get_decision() { 
//   Decision dec = get_solver()->decisions.back(0); 
//   dec.invert();
//   return dec;
// }

void Mistral::GlobalConstraint::initialise_vars(Solver *s) {
  for(unsigned int i=0; i<scope.size; ++i) {
    scope[i].initialise(s, 1);

    //std::cout << "INITIALISE " << scope[i] << " => " << get_solver()->variables[scope[i].id()] << std::endl;

  }
}

void Mistral::ConstraintImplementation::initial_post(Solver *s) {


#ifdef _DEBUG_BUILD
  std::cout << "[" << std::setw(4) << id << "]: post on: " ;
#endif

#ifdef _DEBUG_RELAX
  std::cout << "[" << std::setw(4) << id << "]: first post on: " ;
#endif

  solver = s;
  // for each of its variables
  self = new Constraint[on.size];
  index = new int[on.size];

  // First we go through the variables to check whether they are ground.
  // If so, we "desactivate" the corresponding var index.
  // Also, if there is only one active variable and the constraint enforces nfc1, we do not post it at all.
  int nb_actives = on.size;
  for(unsigned int i=0; i<on.size; ++i) {
    index[i] = -1;
    self[i] = Constraint(this, i|type);
    if(_scope[i].is_ground()) {
      --nb_actives;
      desactivate(i);
    }
  }

  // Now we post the constraint on the active variables (provided that there are at least 2)
  if(!enforce_nfc1 || nb_actives>1) {
    Constraint c;

    for(unsigned int i=0; i<on.size; ++i) {
      if(!(_scope[i].is_ground())) {
	
        c = self[i];
	c.data |= POSTED;

	solver->save( c );

#ifdef _DEBUG_BUILD
	std::cout << _scope[i] << " " ;
#endif

#ifdef _DEBUG_RELAX
	std::cout << _scope[i] << " " ;
#endif

	//un_relax_from(i);
	check_and_un_relax_from(i);
      }  
    }
  }

#ifdef _DEBUG_BUILD
  std::cout << std::endl;
#endif

#ifdef _DEBUG_RELAX
  std::cout << std::endl;
#endif

  //mark_domain();
}

int Mistral::ConstraintImplementation::get_trigger_type(const int i) {
  return (int)(on[i] - &(get_solver()->constraint_graph[_scope[i].id()].on[0]));
}
 
void Mistral::ConstraintImplementation::set_scope(const int i, Variable x) {
  // std::cout << "set scope[" << i << "] of ";
  // display(std::cout);
  // std::cout << " was " << _scope[i] << " now " << x << std::endl;

  // std::cout << "previous trigger list: " << on[i] << " / " << (int)(on[i] - &get_solver()->constraint_graph[_scope[i].id()].on[0]) << std::endl; 



  // std::cout << "trigger was: " << on[i] << " re-link to " ;

  // std::cout << "\njjj: " << get_solver()->constraint_graph[x.id()].on[0] << std::endl;
  // std::cout << "\njjj: " << get_solver()->constraint_graph[x.id()].on[1] << std::endl;
  // std::cout << "\njjj: " << get_solver()->constraint_graph[x.id()].on[2] << std::endl;

  // std::cout << get_trigger_type(i) << std::endl;

  on[i] = &(get_solver()->constraint_graph[x.id()].on[get_trigger_type(i)]);


  //std::cout << on[i] << std::endl;


  _scope.stack_[i] = x;

  // on[i] = 
  // on.add(&(get_solver()->constraint_graph[x.id()].on[t]));
}

int Mistral::Trigger::post(Constraint ct) {
  add(ct);
  return size-1;
}

int Mistral::Trigger::check_and_post(Constraint ct) {
  if(!size || back() != ct) add(ct);
  return size-1;
}


void Mistral::Trigger::relax(const int idx) {
  //std::cout << "relax " << idx << " out of " << size << std::endl;
  if(idx != (int)(size-1)) stack_[size-1].set_rank(idx);
  remove(idx);
}




Mistral::PropagationOutcome Mistral::BinaryConstraint::propagate() {

#ifdef _DEBUG_GENPROPAG
  std::cout << "domain propagate" << this << std::endl;
#endif


  if(!support[0]) initialise_supports();

  PropagationOutcome wiped = CONSISTENT; 
  for(int i=0; IS_OK(wiped) && i<2; ++i) {
    wiped = BinaryConstraint::propagate(i, RANGE_EVENT);
  }
  return wiped;

}


Mistral::PropagationOutcome Mistral::BinaryConstraint::bound_propagate() {

#ifdef _DEBUG_GENPROPAG
  std::cout << "bound propagate" << this << std::endl;
#endif

  if(!support[0]) initialise_supports();

  PropagationOutcome wiped = CONSISTENT; 
  for(int i=0; IS_OK(wiped) && i<2; ++i) {
    wiped = BinaryConstraint::bound_propagate(i, RANGE_EVENT);
  }
  return wiped;

}


Mistral::PropagationOutcome Mistral::BinaryConstraint::bound_propagate(const int changed_idx, const Event evt) {

#ifdef _DEBUG_GENPROPAG
  std::cout << "revise the bounds of " << scope[1-changed_idx] << std::endl;
#endif

  PropagationOutcome wiped = CONSISTENT; 
  int vali, vnext;
  int revise_idx = 1-changed_idx;
  
  if(RANGE_CHANGED(evt)) {
    
#ifdef _DEBUG_GENPROPAG
    std::cout << " - (bc) revise " << scope[revise_idx] << " in " << scope[revise_idx].get_domain() 
	      << " w.r.t. " << scope[changed_idx] 
	      << std::endl;
#endif

    if(!support[0]) initialise_supports();
    
    vnext = scope[revise_idx].get_min();
    do {
      vali = vnext;
      vnext = scope[revise_idx].next(vali);
      
#ifdef _DEBUG_GENPROPAG
      std::cout << "  *find a support for " << scope[revise_idx] << " = " << vali << std::endl;
#endif
      
      if( !(scope[changed_idx].contain(support[revise_idx][vali][changed_idx])) &&
	  !find_bound_support(revise_idx, vali) ) {
	
#ifdef _DEBUG_GENPROPAG
	std::cout << "  => none found!" << std::endl;
#endif
	
	if(IS_FAIL(scope[revise_idx].remove(vali))) {
	  wiped = FAILURE(revise_idx);
	} 
      }
      else { 
#ifdef _DEBUG_GENPROPAG
     	std::cout << "  => ok" << std::endl;
#endif
	break;
      } 
    } while( vali < vnext );
    
    if( IS_OK(wiped) && vali < vnext ) {

      vnext = scope[revise_idx].get_max();
      do {
	vali = vnext;
	vnext = scope[revise_idx].prev(vali);
	
#ifdef _DEBUG_GENPROPAG
	std::cout << "  *find a support for " << scope[revise_idx] << " = " << vali << std::endl;
#endif
      
	if( !(scope[changed_idx].contain(support[revise_idx][vali][changed_idx])) &&
	    !find_bound_support(revise_idx, vali) ) {
	
#ifdef _DEBUG_GENPROPAG
	  std::cout << "  => none found!" << std::endl;
#endif
	  
	  if(IS_FAIL(scope[revise_idx].remove(vali))) {
	    wiped = FAILURE(revise_idx);
	  } 
	}
	else { 
#ifdef _DEBUG_GENPROPAG
	  std::cout << "  => ok" << std::endl;
#endif
	  break;
	} 
      } while( vali > vnext );
      
    }
    
#ifdef _DEBUG_GENPROPAG
    if(!(IS_OK(wiped))) {
      std::cout << "FAIL!!" << std::endl;
    }
#endif
  }

  return wiped;
}


Mistral::PropagationOutcome Mistral::BinaryConstraint::propagate(const int changed_idx, const Event evt) {

#ifdef _DEBUG_GENPROPAG
  std::cout << "revise the domain of " << scope[1-changed_idx] << std::endl;
#endif

  PropagationOutcome wiped = CONSISTENT; 
  int vali, vnext;
  int revise_idx = 1-changed_idx;

#ifdef _DEBUG_GENPROPAG
    std::cout << " - revise " << scope[revise_idx] << " in " << scope[revise_idx].get_domain() 
	      << " w.r.t. " << scope[changed_idx] 
	      << std::endl;
#endif

  if(!support[0]) initialise_supports();

  vnext = scope[revise_idx].get_first();
  do {
    vali = vnext;
    vnext = scope[revise_idx].next(vali);

#ifdef _DEBUG_GENPROPAG
    std::cout << "  *find a support for " << scope[revise_idx] << " = " << vali << std::endl;
#endif

    if( !(scope[changed_idx].contain(support[revise_idx][vali][changed_idx])) &&
	!find_support(revise_idx, vali) ) {

#ifdef _DEBUG_GENPROPAG
	std::cout << "  => none found!" << std::endl;
#endif

      if(IS_FAIL(scope[revise_idx].remove(vali))) {
	wiped = FAILURE(revise_idx);
      } 
    } 
#ifdef _DEBUG_GENPROPAG
      else {
	std::cout << "  => ok" << std::endl;
      }
#endif

  } while( vali < vnext );

#ifdef _DEBUG_GENPROPAG
  if(!(IS_OK(wiped))) {
    std::cout << "FAIL!!" << std::endl;
  }
#endif

  return wiped;
}


// void Mistral::BinaryConstraint::use_residual_support() {
//   support = new bituple*[2];
//   for(int var=0; var<2; ++var) {
//     support[var] = new int*[]
//   }
// }

bool Mistral::BinaryConstraint::find_support(const int revise_idx, const int vli) 
{
  int changed_idx = 1-revise_idx, vali = -INFTY; //, *solution = support[revise_idx][vli];

  solution[revise_idx] = vli;
  solution[changed_idx] = scope[changed_idx].get_first();

  //std::cout << "   " << solution[0] << " " << solution[1] << std::endl;

  while( vali<solution[changed_idx] && check( solution ) ) {

    vali = solution[changed_idx];
    solution[changed_idx] = scope[changed_idx].next( vali );

    //if(vali<solution[changed_idx]) std::cout << "   " << solution[0] << " " << solution[1] << std::endl;

  } 
  return vali<solution[changed_idx];
}


// bool print_sol(int *sol, Mistral::Variable var, int val, Mistral::Variable against, 
// 	       int id, Mistral::BinaryConstraint* c) {
//   std::cout << "check [" << id << "] " << var << " in " << var.get_domain() << " = " << val 
// 	    << " against " << against << " in " << against.get_domain() << " | ";
//   c->display(std::cout);
//   std::cout << ": {" << sol[0] << ", " << sol[1] << "}\n";
//   return true;
// }


bool Mistral::BinaryConstraint::find_bound_support(const int revise_idx, const int vli) 
{
  int changed_idx = 1-revise_idx;
  int max_val = scope[changed_idx].get_max(); 

  solution[revise_idx] = vli;
  
  for( solution[changed_idx] = scope[changed_idx].get_min(); 
       //print_sol(solution, scope[revise_idx], vli, scope[changed_idx], id, this) &&
	 solution[changed_idx] <= max_val && check( solution );
       ++solution[changed_idx]);
  
  return solution[changed_idx]<=max_val;
}


Mistral::PropagationOutcome Mistral::TernaryConstraint::propagate() {
  if(!support[0]) initialise_supports();

  PropagationOutcome wiped = CONSISTENT; 

  for(int i=0; IS_OK(wiped) && i<3; ++i) {
    wiped = TernaryConstraint::propagate(i, RANGE_EVENT);
  }

  return wiped;
}

Mistral::PropagationOutcome Mistral::TernaryConstraint::bound_propagate() {
  if(!support[0]) initialise_supports();

  PropagationOutcome wiped = CONSISTENT; 

  for(int i=0; IS_OK(wiped) && i<3; ++i) {
    wiped = TernaryConstraint::bound_propagate(i, RANGE_EVENT);
  }

  return wiped;
}

Mistral::PropagationOutcome Mistral::TernaryConstraint::propagate(const int changed_idx, const Event evt) {
  PropagationOutcome wiped = CONSISTENT; 
  int vali, vnext;
  int revise_idx, other_idx;
  int x, y;

  if(!support[0]) initialise_supports();


#ifdef _DEBUG_GENPROPAG
  std::cout << " propagate " << event2str(evt) << " on " << scope[changed_idx] << std::endl;
#endif


  for(int i=1; i<3; ++i) {
    revise_idx = (changed_idx+i)%3;
    other_idx = (changed_idx+3-i)%3;


#ifdef _DEBUG_GENPROPAG
    std::cout << " -revise " << scope[revise_idx] << " in " << scope[revise_idx].get_domain() 
	      << " w.r.t. " << scope[changed_idx] << " & " << scope[other_idx]
	      << std::endl;
#endif

    vnext = scope[revise_idx].get_min();

    //    std::cout << vnext << std::endl;

    do {
      vali = vnext;
      vnext = scope[revise_idx].next(vali);

#ifdef _DEBUG_GENPROPAG
      std::cout << "  *find a support for " << scope[revise_idx] << " = " << vali << std::endl;
      std::cout.flush();
#endif
      x = support[revise_idx][vali][changed_idx];
      y = support[revise_idx][vali][other_idx];


      // std::cout << std::endl << x << " " << y << std::endl;


      
      if( (x == NOVAL 
	   || y == NOVAL
	   || !scope[changed_idx].contain(x)
	   || !scope[other_idx].contain(y)
	   ) &&
	  !find_support(revise_idx, vali) ) {

#ifdef _DEBUG_GENPROPAG
	std::cout << "  => none found!" << std::endl;
#endif

	if(IS_FAIL(scope[revise_idx].remove(vali))) {
	  wiped = FAILURE(revise_idx);
	} 
      } 
#ifdef _DEBUG_GENPROPAG
      else {
	std::cout << "  => ok" << std::endl;
      }
#endif

    } while( vali < vnext );
  }

#ifdef _DEBUG_GENPROPAG
  if(!(IS_OK(wiped))) {
    std::cout << "FAIL!!" << std::endl;
  }
#endif

  return wiped;
}



Mistral::PropagationOutcome Mistral::TernaryConstraint::bound_propagate(const int changed_idx, const Event evt) {
  PropagationOutcome wiped = CONSISTENT; 
  int vali, vnext;
  int revise_idx, other_idx;
  int x, y;

  if(!support[0]) initialise_supports();

#ifdef _DEBUG_GENPROPAG
  std::cout << " bound propagate " << event2str(evt) << " on " << scope[changed_idx] << std::endl;
#endif


  for(int i=1; i<3; ++i) {
    revise_idx = (changed_idx+i)%3;
    other_idx = (changed_idx+3-i)%3;


#ifdef _DEBUG_GENPROPAG
    std::cout << " - (bc) revise " << scope[revise_idx] << " in " << scope[revise_idx].get_domain() 
	      << " w.r.t. " << scope[changed_idx] << " & " << scope[other_idx]
	      << std::endl;
#endif

    vnext = scope[revise_idx].get_min();
    do {
      vali = vnext;
      vnext = scope[revise_idx].next(vali);

#ifdef _DEBUG_GENPROPAG
      std::cout << "  *find a support for " << scope[revise_idx] << " = " << vali << std::endl;
      std::cout.flush();
#endif
      x = support[revise_idx][vali][changed_idx];
      y = support[revise_idx][vali][other_idx];
      
      if( (x == NOVAL 
	   || y == NOVAL
	   || !scope[changed_idx].contain(x)
	   || !scope[other_idx].contain(y)
	   ) &&
	  !find_bound_support(revise_idx, vali) ) {

#ifdef _DEBUG_GENPROPAG
	std::cout << "  => none found!" << std::endl;
#endif

	if(IS_FAIL(scope[revise_idx].remove(vali))) {
	  wiped = FAILURE(revise_idx);
	} 
      } 
      else {
#ifdef _DEBUG_GENPROPAG
	std::cout << "  => ok" << std::endl;
#endif
	break;
      }
    } while( vali < vnext );

    if( IS_OK(wiped) && vali < vnext ) {

      vnext = scope[revise_idx].get_max();
      do {
	vali = vnext;
	vnext = scope[revise_idx].prev(vali);

#ifdef _DEBUG_GENPROPAG
	std::cout << "  *find a support for " << scope[revise_idx] << " = " << vali << std::endl;
	std::cout.flush();
#endif
	x = support[revise_idx][vali][changed_idx];
	y = support[revise_idx][vali][other_idx];
	
	if( (x == NOVAL 
	     || y == NOVAL
	   || !scope[changed_idx].contain(x)
	     || !scope[other_idx].contain(y)
	     ) &&
	    !find_bound_support(revise_idx, vali) ) {
	  
#ifdef _DEBUG_GENPROPAG
	  std::cout << "  => none found!" << std::endl;
#endif
	  
	  if(IS_FAIL(scope[revise_idx].remove(vali))) {
	    wiped = FAILURE(revise_idx);
	  } 
	} 
	else {
#ifdef _DEBUG_GENPROPAG
	  std::cout << "  => ok" << std::endl;
#endif
	  break;
	}
      } while( vali > vnext );
    }
  }

#ifdef _DEBUG_GENPROPAG
  if(!(IS_OK(wiped))) {
    std::cout << "FAIL!!" << std::endl;
  }
#endif

  return wiped;
}

bool Mistral::TernaryConstraint::find_support(const int revise_idx, const int vli) 
{
  bool no_support = true;

  int x_idx = (revise_idx+1)%3, y_idx = (revise_idx+2)%3;

  int valx = scope[x_idx].get_min(), valy; //, *solution = support[revise_idx][vli];
  
  solution[revise_idx] = vli;


  do {
    solution[x_idx] = valx;
    valy = scope[y_idx].get_min();

    do {
      solution[y_idx] = valy;

#ifdef _DEBUG_GENPROPAG
      std::cout << "    " << solution[0] << " " << solution[1] << " " << solution[2] << std::endl;
#endif

      no_support = check(solution); 

      if(no_support) 
	valy = scope[y_idx].next( valy );

    } while( solution[y_idx] < valy );
    
    if(no_support)
      valx = scope[x_idx].next( valx );

  } while( solution[x_idx] < valx );


  return !no_support;
}


bool Mistral::TernaryConstraint::find_bound_support(const int revise_idx, const int vli) 
{
  bool no_support = true;

  int x_idx = (revise_idx+1)%3, y_idx = (revise_idx+2)%3;


  int min_y_val = scope[y_idx].get_min();
  int max_y_val = scope[y_idx].get_max();
  int max_x_val = scope[x_idx].get_max();
  
  solution[revise_idx] = vli;


  for(solution[x_idx] = scope[x_idx].get_min();
      no_support && solution[x_idx] <= max_x_val;
      ++solution[x_idx]
      ) {
    for(solution[y_idx] = min_y_val;
	no_support && solution[y_idx] <= max_y_val;
	++solution[y_idx]
	) {
    
#ifdef _DEBUG_GENPROPAG
      std::cout << "    " << solution[0] << " " << solution[1] << " " << solution[2] << std::endl;
#endif

      no_support = check(solution); 
      
    }
  }

  return !no_support;
}


void Mistral::BinaryConstraint::trigger()  { get_solver()->active_constraints.trigger(this); }
void Mistral::TernaryConstraint::trigger() { get_solver()->active_constraints.trigger(this); }
void Mistral::GlobalConstraint::trigger()  { get_solver()->active_constraints.trigger(this); }


Mistral::GlobalConstraint::GlobalConstraint(Vector< Variable > scp) {
  for(unsigned int i=0; i<scp.size; ++i) scope.add(scp[i]);
}
Mistral::GlobalConstraint::GlobalConstraint(std::vector< Variable > scp) {
  for(std::vector< Variable >::iterator vi=scp.begin(); vi!=scp.end(); ++vi) scope.add(*vi);
}
Mistral::GlobalConstraint::GlobalConstraint(Variable* scp, const int n) {
  for(int i=0; i<n; ++i) scope.add(scp[i]);
}

int Mistral::GlobalConstraint::get_backtrack_level() {
  return solver->level-1;
}

Mistral::Decision Mistral::GlobalConstraint::get_decision() { 
  Decision dec = get_solver()->decisions.back(0); 
  dec.invert();
  return dec;
}


Mistral::PropagationOutcome Mistral::GlobalConstraint::propagate(const int changed_idx, const Event evt) {
  PropagationOutcome wiped = CONSISTENT; 
  unsigned int i;
  int vali, vnext;

  for( i=0; IS_OK(wiped) && i<scope.size; ++i ) {
    if( (int)i != changed_idx ) { 
      vnext = scope[i].get_first();
      do {
	vali = vnext;
	vnext = scope[i].next(vali);
	
#ifdef _DEBUG_GENPROPAG
	std::cout << "find a support for " << scope[i] << " = " << vali << std::endl;
#endif

	if( ( !first_support(i, vali) && !find_support(i, vali) ) ) {
	  if(IS_FAIL(scope[i].remove(vali))) {
	    wiped = FAILURE(i);
	  } 
	}
      } while( vali < vnext );
    }	
  }
  return wiped;
}

Mistral::PropagationOutcome Mistral::GlobalConstraint::propagate() {
  PropagationOutcome wiped = CONSISTENT; 
  unsigned int i, j;
  int vali; //, vnext;

#ifdef _DEBUG_GENPROPAG
  if(_DEBUG_GENPROPAG) {
  std::cout << "propagate " << this << std::endl;
  for(i=0; i<scope.size; ++i)
    {
      std::cout << scope[i] << " in " << scope[i].get_domain() << std::endl;
      
      vnext = scope[i].get_first();
      do {
  	vali = vnext;
  	vnext = scope[i].next(vali);
	
  	std::cout << " " << vali ;
	
      } while( vali != vnext );
      
      std::cout << std::endl;

      Domain dom_xi(scope[i]);
      Domain::iterator xend = dom_xi.begin();
      Domain::iterator xit = dom_xi.end();
      
      while(xit != xend) {
  	--xit;

  	std::cout << " " << dom_xi.get_value(xit) ;
      }

      std::cout << std::endl << std::endl;

     }
  }
#endif


  while(!changes.empty()) {
    j = changes.pop();
    for( i=0; IS_OK(wiped) && i<scope.size; ++i ) {
      if( i != j ) { 

	// std::cout << "std: " << scope[i] << " " << scope[i].get_domain(); 
	// vnext = scope[i].get_first();
	// do {
	//   vali = vnext;
	//   vnext = scope[i].next(vali);

	//   std::cout << " " << vali ;

	// } while( vali != vnext );
	// std::cout << std::endl;


	// std::cout << "itr: " << scope[i] << " " << scope[i].get_domain(); 
	// Domain dom_x(scope[i]);
	// Domain::iterator xt = dom_x.end();
	// Domain::iterator xnd = dom_x.begin();

	// while(xt != xnd) {
	//   vali = dom_x.get_value(--xt);

	//   std::cout << " " << vali ;

	// }
	// std::cout << std::endl << std::endl;


// 	vnext = scope[i].get_first();
// 	do {
// 	  vali = vnext;
// 	  vnext = scope[i].next(vali);

// #ifdef _DEBUG_GENPROPAG
// 	  if(_DEBUG_GENPROPAG) {
// 	  // if(solver->parameters.verbosity) {
// 	  std::cout << "find a support for " << scope[i] << " = " << vali << std::endl;
// 	  // }
// 	  }
// #endif

// 	  if( ( !first_support(i, vali) && !find_support(i, vali) ) ) {
// 	    if(IS_FAIL(scope[i].remove(vali))) {
// 	      wiped = FAILURE(i);
// 	    } else if(changes.list_ == events.list_) {
// 	      if(!changes.contain(i)) changes.add(i);
// 	    }
// 	  }
// 	} while( vali != vnext );


	Domain dom_xi(scope[i]);
	Domain::iterator xit = dom_xi.end();
	Domain::iterator xend = dom_xi.begin();

	while(xit != xend) {
	  vali = dom_xi.get_value(--xit);

#ifdef _DEBUG_GENPROPAG
	  if(_DEBUG_GENPROPAG) {
	    // if(solver->parameters.verbosity) {
	    std::cout << "find a support for " << scope[i] << " = " << vali << std::endl;
	    // }
	  }
#endif

	  if( ( !first_support(i, vali) && !find_support(i, vali) ) ) {
	    if(IS_FAIL(scope[i].remove(vali))) {
	      wiped = FAILURE(i);
	    } else if(changes.list_ == events.list_) {
	      if(!changes.contain(i)) changes.add(i);
	    }
	  }
	}

      }	
    }
  }
  return wiped;
}


Mistral::PropagationOutcome Mistral::GlobalConstraint::bound_propagate(const int changed_idx, const Event evt) {
  PropagationOutcome wiped = CONSISTENT; 
  unsigned int i;


  int vali, valmax;//, vnext;
  bool supported;
  for( i=0; IS_OK(wiped) && i<scope.size; ++i ) {
    if( (int)i != changed_idx ) { 
      supported = false;
      vali = scope[i].get_min();
      valmax = scope[i].get_max();
      while(!supported && IS_OK(wiped) && vali<=valmax) {
	
	//std::cout << "find support for " << scope[i] << "=" << vali << std::endl;
	
	if( ( !first_support(i, vali) && !find_bound_support(i, vali) ) ) {
	  if(IS_FAIL(scope[i].remove(vali))) {
	    wiped = FAILURE(i);
	  } else if(changes.list_ == events.list_) {
	    if(!changes.contain(i)) changes.add(i);
	  }
	} else supported = true;
	
	++vali;
      }
      
      if(supported && vali<=valmax) {
	supported = false;
	vali = valmax;
	while(!supported && IS_OK(wiped)) {
	  
	  //std::cout << "find support for " << scope[i] << "=" << vali << std::endl;
	  
	  if( ( !first_support(i, vali) && !find_bound_support(i, vali) ) ) {
	    if(IS_FAIL(scope[i].remove(vali))) {
		wiped = FAILURE(i);
	    }  else if(changes.list_ == events.list_) {
	      if(!changes.contain(i)) changes.add(i);
	    }
	  } else supported = true;
	  --vali;
	}
      }
    }	
  }
  return wiped;
}


Mistral::PropagationOutcome Mistral::GlobalConstraint::bound_propagate() {
  PropagationOutcome wiped = CONSISTENT;
  
  unsigned int i, j;

  int vali, valmax;//, vnext;
  bool supported;

  while(!changes.empty()) {
    j = changes.pop();

#ifdef _DEBUG_GENPROPAG
  std::cout << " propagate " << event2str(event_type[j]) << " on " << scope[j] << std::endl;
#endif

    for( i=0; IS_OK(wiped) && i<scope.size; ++i ) {
      //if( i != j ) { 

#ifdef _DEBUG_GENPROPAG
    std::cout << " - (bc) revise " << scope[i] << " in " << scope[i].get_domain() 
	      << " w.r.t. " << scope[j] 
	      << std::endl;
#endif

	supported = false;
	vali = scope[i].get_min();
	valmax = scope[i].get_max();
	while(!supported && IS_OK(wiped) && vali<=valmax) {

#ifdef _DEBUG_GENPROPAG
	  std::cout << "  *find a support for " << scope[i] << " = " << vali << std::endl;
	  std::cout.flush();
#endif
	  
	  if( ( !first_support(i, vali) && !find_bound_support(i, vali) ) ) {
	    
#ifdef _DEBUG_GENPROPAG
	    std::cout << "  => none found!" << std::endl;
#endif

	    if(IS_FAIL(scope[i].remove(vali))) {
	      wiped = FAILURE(i);
	    } else if(changes.list_ == events.list_) {
	      if(!changes.contain(i)) changes.add(i);
	    }
	  } else {

#ifdef _DEBUG_GENPROPAG
	    std::cout << "  => ok" << std::endl;
#endif

	    supported = true;
	  }
	  ++vali;
	}

	if(supported && vali<=valmax) {
	  supported = false;
	  vali = valmax;
	  while(!supported && IS_OK(wiped)) {

#ifdef _DEBUG_GENPROPAG
	  std::cout << "  *find a support for " << scope[i] << " = " << vali << std::endl;
	  std::cout.flush();
#endif

	    if( ( !first_support(i, vali) && !find_bound_support(i, vali) ) ) {

#ifdef _DEBUG_GENPROPAG
	  std::cout << "  *find a support for " << scope[i] << " = " << vali << std::endl;
	  std::cout.flush();
#endif

	      if(IS_FAIL(scope[i].remove(vali))) {
		wiped = FAILURE(i);
	      }  else if(changes.list_ == events.list_) {
		if(!changes.contain(i)) changes.add(i);
	      }
	    } else {

#ifdef _DEBUG_GENPROPAG
	      std::cout << "  => ok" << std::endl;
#endif
	      
	      supported = true;
	    }
	    --vali;
	  }
	}
	//}	
    }
  }
  
  return wiped;
}


bool Mistral::GlobalConstraint::first_support(const int vri, const int vli) 
{
  int j;
  if( supports && supports[vri][vli][0] != NOVAL ) {
    j=scope.size;
    while( j-- ) 
      if( vri != j )
	if (!scope[j].contain( supports[vri][vli][j] )) break;
    if( j < 0 ) 
      return true;
  } 
  j=scope.size;
  while( j-- ) 
    solution[j] = scope[j].get_first();
  solution[vri] = vli; 

  return false;
}

bool Mistral::GlobalConstraint::find_support(const int vri, const int vli) 
{
  int i=scope.size, vali;
  bool found=false;
  // sol is initialized: either to the value 
  // a variable is already assigned to
  // or to the first value in its domain
  while(i >= 0) {

#ifdef _DEBUG_GENPROPAG
    if(_DEBUG_GENPROPAG) {
    // if(solver->parameters.verbosity) {
      std::cout << "\t<" << solution[0] ;
      for(unsigned int k=1; k<scope.size; ++k) {
    	std::cout << " " << solution[k];
      }
      std::cout << "> ";
    // }
    }
#endif

    // check this assignment
    if( !check( solution ) ) {

#ifdef _DEBUG_GENPROPAG
      if(_DEBUG_GENPROPAG) {
      // if(solver->parameters.verbosity) {
      std::cout << "OK!" << std::endl;
      // }
      }
#endif

      found=true;
      if( supports ) {
	vali = scope.size;
	while( vali-- )
	  supports[vri][vli][vali] = solution[vali];
      }
      break;
    } 

#ifdef _DEBUG_GENPROPAG
    else 
      if(_DEBUG_GENPROPAG) {
      //if(solver->parameters.verbosity) {
      std::cout << "NO" << std::endl;
      }
    //     }
#endif
 
    // try to assign more things
    // find the last var whose domain we have not exhausted
    --i;
    while( i >= 0 ) {
      if( i == vri || scope[i].is_ground() ) {
	--i;
	continue;
      }
      vali = scope[i].next( solution[i] );
      if(vali != solution[i]) {
	solution[i] = vali;
      	break;
      } else {
	solution[i] = scope[i].get_first();
      }
      --i;
    }
    if( i >= 0 )
      i = scope.size;
  } 
  return found;
}


bool Mistral::GlobalConstraint::find_bound_support(const int vri, const int vli) 
{
  int i=scope.size, vali;
  bool found=false;
  // sol is initialized: either to the value 
  // a variable is already assigned to
  // or to the first value in its domain
  while(i >= 0) {


    // std::cout << "\t<" << solution[0] ;
    // for(unsigned int k=1; k<scope.size; ++k) {
    //   std::cout << " " << solution[k];
    // }
    // std::cout << "> ";

    // check this assignment
    if( !check( solution ) ) {

      //std::cout << "OK!" << std::endl;

      found=true;
      if( supports ) {
	vali = scope.size;
	while( vali-- )
	  supports[vri][vli][vali] = solution[vali];
      }
      break;
    } //  else {
    //   std::cout << "NO" << std::endl;
    // }
 
    // try to assign more things
    // find the last var whose domain we have not exhausted
    --i;
    while( i >= 0 ) {
      if( i == vri || scope[i].is_ground() ) {
	--i;
	continue;
      }
      if(solution[i]>=scope[i].get_max())
	solution[i] = scope[i].get_min();
      else {
	++solution[i];
      	break;
      }
      --i;
    }
    if( i >= 0 )
      i = scope.size;
  } 
  return found;
}


Mistral::GlobalConstraint::~GlobalConstraint() {
#ifdef _DEBUG_MEMORY
  std::cout << "c delete global constraint" << std::endl;
#endif
  if(changes.list_ == events.list_)
    events.list_ = NULL;
  delete [] event_type;
  delete [] solution;
  //delete [] scope;
}

void Mistral::GlobalConstraint::initialise() {
  // trail_.initialise(0,2*scope.size);
  // trail_.add(-1);

#ifdef _DEBUG_BUILD
  std::cout << "[" << std::setw(4) << id << "]: initialise (global)" << std::endl;
#endif



  event_type = new Event[on.size];
  // self = new int[scope.size];
  // index = new Constraint**[scope.size];
  // trigger = new int[scope.size];
  solution = new int[scope.size];
  changes.initialise(0, scope.size-1, scope.size, false);
  supports = NULL;

  std::fill(event_type, event_type+on.size, NO_EVENT);
  // std::fill(self, self+scope.size, -1);
  // std::fill(index, index+scope.size, (Constraint**)NULL);
  // std::fill(trigger, trigger+scope.size, _DOMAIN_);
  std::fill(solution, solution+scope.size, 0);

  active.initialise(solver, 0, on.size-1, on.size, true);

  GlobalConstraint::set_idempotent();
}

void Mistral::GlobalConstraint::initialise_activity(double *lact, double *vact, double norm) {
  int i=scope.size, idx;;
  double w = norm/i;
  while(i--) {
    idx = scope[i].id();
    vact[idx] += w;
    lact[2*idx] += w/2;
    lact[2*idx+1] += w/2;
  }
}



std::ostream& Mistral::ConstraintImplementation::display(std::ostream& os) const {
  return os;
}

std::ostream& Mistral::GlobalConstraint::display(std::ostream& os) const {
  os << name() << "(" << scope[0];
  for(unsigned int i=1; i<scope.size; ++i)
    os << ", " << scope[i]/*.get_var()*/;
  os << ")";
  return os;
}

void Mistral::ConstraintNotEqual::initialise() {
  ConstraintImplementation::initialise();

  //std::cout << "TYPE AT INITIALISATION  " << type << std::endl;

  trigger_on(_VALUE_, scope[0]);
  trigger_on(_VALUE_, scope[1]);
  //set_idempotent(true);
  //postponed = false;
}

void Mistral::ConstraintNotEqual::mark_domain() {
  get_solver()->forbid(scope[0].id(), RANGE_VAR);
  get_solver()->forbid(scope[1].id(), RANGE_VAR);
  // get_solver()->mark_non_convex(scope[0].id());
  // get_solver()->mark_non_convex(scope[1].id());
}

Mistral::PropagationOutcome Mistral::ConstraintNotEqual::rewrite() {
  RewritingOutcome r_evt = NO_EVENT; 

  if(scope[0].is_ground() || scope[1].is_ground() || !(scope[0].intersect(scope[1]))) {

#ifdef _DEBUG_REWRITE
    std::cout << "    relax ";
    display(std::cout);
    std::cout << std::endl;
#endif

    r_evt = SUPPRESSED;
    relax();
  } else {

    if( scope[0].get_min() == 0 && 
	scope[1].get_min() == 0 &&
	scope[0].get_max() == 1 && 
	scope[1].get_max() == 1 &&
	scope[0].is_expression() &&
	scope[1].is_expression() ) {
      
      r_evt = SUPPRESSED;
      
#ifdef _DEBUG_REWRITE
      std::cout << "    relax ";
      display(std::cout);
      std::cout << std::endl;
#endif
      
      relax();
      
      Constraint con = Constraint( new PredicateNot(scope[0], scope[1]) );
      
#ifdef _DEBUG_REWRITE
      std::cout << "    post " << con << std::endl;
#endif

      get_solver()->add( con );
    }
  }  

  return r_evt;
}


Mistral::PropagationOutcome Mistral::ConstraintNotEqual::propagate() {

  // std::cout << "ACTIVE: " << active <<  " " ;
  // std::cout.flush();
  // std::cout << scope[0].get_domain() << " " << scope[1].get_domain() << std::endl;

  PropagationOutcome wiped_idx = CONSISTENT;
  if(!active) {
    if(scope[0].get_first() == scope[1].get_first()) wiped_idx = FAILURE(0);
  } else {
    
    // if(active != 3) {
    //   std::cout << "remove the value of " << scope[2-active] << ": " 
    // 		<< scope[2-active].get_domain() << " from " 
    // 		<< scope[active-1] << " in " << scope[active-1].get_domain() << std::endl;
    // }

    
    //print_active();
    
    // if(active != 3)
    //   std::cout << "remove " << scope[2-active].get_domain() << " from " 
    // 		<< scope[active-1].get_domain() << std::endl;


    if(active != 3 && (scope[active-1].remove(scope[2-active].get_first()) == FAIL_EVENT)) wiped_idx = FAILURE(active-1);
  }
  return wiped_idx;
}

Mistral::PropagationOutcome Mistral::ConstraintNotEqual::propagate(const int changed_idx, 
								   const Event evt) {

  // std::cout << "\n propagate changes on " << scope[changed_idx] << " to " << scope[1-changed_idx] 
  // 	    << " (remove " << scope[changed_idx].get_first() << " from " 
  // 	    << scope[1-changed_idx].get_domain() << ")" << std::endl;

  //active^=(1 << changed_idx);
  int var = 1-changed_idx;
  PropagationOutcome wiped_idx = (scope[var].remove(scope[changed_idx].get_first()) == FAIL_EVENT ? FAILURE(var) : CONSISTENT);
  //if(IS_OK(wiped_idx)) relax_from(var);
  return wiped_idx;
}

std::ostream& Mistral::ConstraintNotEqual::display(std::ostream& os) const {
  os << scope[0]/*.get_var()*/ << " =/= " << scope[1]/*.get_var()*/;
  return os;
}

void Mistral::ConstraintEqual::initialise() {
  ConstraintImplementation::initialise();

  //Constraint::initialise();
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_DOMAIN_, scope[1]);
  //set_idempotent(true);
}

void Mistral::ConstraintEqual::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
  // get_solver()->mark_non_convex(scope[0].id());
  // get_solver()->mark_non_convex(scope[1].id());
}

Mistral::PropagationOutcome Mistral::ConstraintEqual::rewrite() {

// #ifdef _DEBUG_REWRITE
//       std::cout << "REWRITE EQUALITY " ;
//       display(std::cout);
//       std::cout << std::endl;
//       //std::cout << solver->variables << std::endl;
// #endif

  RewritingOutcome r_evt = SUPPRESSED; 

  //Mistral::PropagationOutcome wiped = propagate(); 


  /// CHANGE THAT:
  // relax all constraints around one of the variables and post clones instead


  // // std::cout << "propag: " << scope[0] << " in " << scope[0].get_domain() 
  // // 	    << " = " << scope[1] << " in " << scope[1].get_domain() << std::endl;


  // // std::cout << 11 << std::endl;

  // // std::cout << scope[0] << std::endl;

  // std::cout << domain2str(scope[0].domain_type) 
  // 	    << " "
  // 	    << (int*)(scope[0].variable)
  // 	    << std::endl;


  // // std::cout << scope[1] << std::endl;

  // std::cout << domain2str(scope[1].domain_type) 
  // 	    << " ";


  // std::cout << (int*)(scope[1].variable)
  // 	    << std::endl;

  // // 

  // std::cout << 11 << std::endl;

  // std::cout << (scope[1].is_ground()) << std::endl;

  // std::cout << 22 << std::endl;

  if( !(scope[1].is_ground()) && !(scope[0].is_ground()) ) {

    //std::cout << 33 << std::endl;

    if( scope[0].is_expression() && scope[1].is_expression() ) {

// #ifdef _DEBUG_REWRITE
//           std::cout << "ACTUALLY REWRITE " ;
//           display(std::cout);
//           std::cout << std::endl;
//           // //std::cout << solver->variables << std::endl;
// #endif 

      relax();

      int k[2], j;

      k[0] = scope[0].id();
      k[1] = scope[1].id();

      j = get_solver()->constraint_graph[k[0]].size() > 
   	get_solver()->constraint_graph[k[1]].size();
      // j is 1 iff degree(x0) > degree(x1), 
      // in that case we want to transfer x1's constraints on x0.
      // x[j] <- x[1-j]


      //std::cout << "k[j]=" << k[j] << std::endl;

      get_solver()->remove(scope[j]);
      //get_solver()->domain_types[k[j]] |= REMOVED_VAR;

#ifdef _DEBUG_REWRITE
      std::cout << "    relax constraints on " << scope[j]/*.get_var()*/ << " and post them on " << scope[1-j]/*.get_var()*/ << std::endl;
#endif

      Constraint con;
      for(Event trig = 0; trig<3; ++trig) {
	for(int i = get_solver()->constraint_graph[k[j]].on[trig].size; i--;) {
	  //for(int t=0; t<3; ++t) {
	  //for(int i=trigger[j]->on[t].size; --i;) {

	  //con = trigger[j]->on[t][i];


	  con = get_solver()->constraint_graph[k[j]].on[trig][i];

#ifdef _DEBUG_REWRITE
	  std::cout << "      relax " << con << std::endl;
#endif


	  con.relax();
	  con.set_scope(con.index(), scope[1-j]);
  	  get_solver()->add(con);


#ifdef _DEBUG_REWRITE
	  std::cout << "      post " << con  << std::endl;
#endif

	  // std::cout << "trig[" << k[1-j] << "][0]: " << get_solver()->constraint_graph[k[1-j]].on[0] << std::endl;
	  // std::cout << "trig[" << k[1-j] << "][1]: " << get_solver()->constraint_graph[k[1-j]].on[1] << std::endl;
	  // std::cout << "trig[" << k[1-j] << "][2]: " << get_solver()->constraint_graph[k[1-j]].on[2] << std::endl;

	  
	  
	}
      }

  //     ConstraintNode nd;
  //     nd = solver->constraint_graph[k[j]]->first(_VALUE_);
  //     while( solver->constraint_graph[k[j]]->next(nd) ) {	
  // 	nd.elt.constraint->relax();
  // 	nd.elt.constraint->scope[nd.elt.index] = scope[1-j];
  // 	solver->add(nd.elt.constraint);
  //     }
      
      //and now scope[j] points to scope[1-j]

      // SELF CHANGE
      //scope[j].expression->self = scope[1-j];
      
      scope[j].expression->id = scope[1-j].id();

// #ifdef _DEBUG_REWRITE
//       //std::cout << solver->variables << std::endl;
//       std::cout << "EQUALITY REWRITEN " ;
//       display(std::cout);
//       std::cout << std::endl;
// #endif

    } 
  } else {
    relax();
  }
  
  return r_evt;
}


Mistral::PropagationOutcome Mistral::ConstraintEqual::propagate() {
  PropagationOutcome wiped = CONSISTENT;
  if(active) {
    if(scope[1].set_domain(scope[0]) == FAIL_EVENT) wiped = FAILURE(1);
    if(IS_OK(wiped) && scope[0].set_domain(scope[1]) == FAIL_EVENT) wiped = FAILURE(0);
  } else if(scope[0].get_min() != scope[1].get_min()) {
    wiped = FAILURE(0);
  }

  return wiped;
}

Mistral::PropagationOutcome Mistral::ConstraintEqual::propagate(const int changed_idx, const Event evt) {
  return(scope[1-changed_idx].set_domain(scope[changed_idx]) == FAIL_EVENT ? FAILURE(1-changed_idx) : CONSISTENT);
}


std::ostream& Mistral::ConstraintEqual::display(std::ostream& os) const {
  os << (scope[0]/*.get_var()*/) << " == " << (scope[1]/*.get_var()*/);
  return os;
}

void Mistral::PredicateUpperBound::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_VALUE_, scope[1]);
}

void Mistral::PredicateUpperBound::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
}


Mistral::Constraint Mistral::PredicateUpperBound::get_negation(const int var, Variable x) { 
  return Constraint( new PredicateLowerBound( scope[0], x, bound-1 ) );
}

Mistral::PropagationOutcome Mistral::PredicateUpperBound::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].is_ground() ) {
    if( scope[1].get_min() ) { // x[0] <= k 
      if( IS_FAIL(scope[0].set_max(bound)) )
	wiped = FAILURE(0);
    } else { // x[0] > k
      if( IS_FAIL(scope[0].set_min(bound+1)) )
	wiped = FAILURE(0);
    } 
  } else {
    if( scope[0].get_min() > bound ) {
      if( IS_FAIL(scope[1].set_domain(0)) ) return FAILURE(1);
    } else if( scope[0].get_max() <= bound ) {
      if( IS_FAIL(scope[1].remove(0)) ) return FAILURE(1);
    }
  }
  
  return wiped;
}


Mistral::PropagationOutcome Mistral::PredicateUpperBound::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( changed_idx ) {
    if( LB_CHANGED(evt) ) { // x[0] <= k 
      if( IS_FAIL(scope[0].set_max(bound)) ) wiped = FAILURE(0);
    } else { // x[0] > k
      if( IS_FAIL(scope[0].set_min(bound+1)) ) wiped = FAILURE(0);
    } 
  } else {
    if( LB_CHANGED(evt) && scope[0].get_min() > bound ) {
      if( IS_FAIL(scope[1].set_domain(0)) ) return FAILURE(1);
    } else if( UB_CHANGED(evt) && scope[0].get_max() <= bound ) {
      if( IS_FAIL(scope[1].remove(0)) ) return FAILURE(1);
    }
  }
  
  return wiped;
}

std::ostream& Mistral::PredicateUpperBound::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " <=> (" << scope[0]/*.get_var()*/ << " <= " << bound << ")";
  return os;
}


void Mistral::PredicateLowerBound::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_VALUE_, scope[1]);
}

void Mistral::PredicateLowerBound::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
}


Mistral::PropagationOutcome Mistral::PredicateLowerBound::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].is_ground() ) {
    if( scope[1].get_min() ) { // x[0] >= k 
      if( IS_FAIL(scope[0].set_min(bound)) )
	wiped = FAILURE(0);
    } else { // x[0] < k
      if( IS_FAIL(scope[0].set_max(bound-1)) )
	wiped = FAILURE(0);
    } 
  } else {
    if( scope[0].get_max() < bound ) {
      if( IS_FAIL(scope[1].set_domain(0)) ) return FAILURE(1);
    } else if( scope[0].get_min() >= bound ) {
      if( IS_FAIL(scope[1].remove(0)) ) return FAILURE(1);
    }
  }
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateLowerBound::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( changed_idx ) {
    if( LB_CHANGED(evt) ) { // x[0] >= k 
      if( IS_FAIL(scope[0].set_min(bound)) )
	wiped = FAILURE(0);
    } else { // x[0] < k
      if( IS_FAIL(scope[0].set_max(bound-1)) )
	wiped = FAILURE(0);
    } 
  } else {
    if( UB_CHANGED(evt) && scope[0].get_max() < bound ) {
      if( IS_FAIL(scope[1].set_domain(0)) ) return FAILURE(1);
    } else if( LB_CHANGED(evt) && scope[0].get_min() >= bound ) {
      if( IS_FAIL(scope[1].remove(0)) ) return FAILURE(1);
    }
  }
  
  return wiped;
}

std::ostream& Mistral::PredicateLowerBound::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " <=> (" << scope[0]/*.get_var()*/ << " >= " << bound << ")";
  return os;
}

void Mistral::PredicateLess::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  trigger_on(_VALUE_, scope[2]);
}

void Mistral::PredicateLess::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}

//x0 + 3 > x1

Mistral::PropagationOutcome Mistral::PredicateLess::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[2].is_ground() ) {
    if( scope[2].get_min() ) { // x[0] + k <= x[1]
      if( IS_FAIL(scope[0].set_max(scope[1].get_max()-offset)) )
	wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_min(scope[0].get_min()+offset)) )
	wiped = FAILURE(1);
    } else if( scope[2].get_max() == 0 ) { 
      // x[0] + k > x[1]
      // x[0] + k - 1 >= x[1]
      // x[1] + (1-k) <= x[0]
      if( IS_FAIL(scope[0].set_min(scope[1].get_min()-offset+1)) )
	wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_max(scope[0].get_max()+offset-1)) )
	wiped = FAILURE(1);
    } 
  } else {
    if( scope[0].get_min() + offset > scope[1].get_max() ) {
      if( IS_FAIL(scope[2].set_domain(0)) ) return FAILURE(2);
    } else if( scope[0].get_max() + offset <= scope[1].get_min() ) {
      if( IS_FAIL(scope[2].remove(0)) ) return FAILURE(2);
    }
  }
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateLess::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;


  // std::cout << "HERE: "  << event2str(evt) << " on " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << std::endl;

  if( changed_idx == 2 ) {
    if( LB_CHANGED(evt) ) {
      // x[0] + offset <= x[1]
      if( IS_FAIL(scope[0].set_max(scope[1].get_max()-offset)) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_min(scope[0].get_min()+offset)) ) wiped = FAILURE(1);
    } else {
      // x[0] + offset > x[1]
      if( IS_FAIL(scope[0].set_min(scope[1].get_min()-offset+1)) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_max(scope[0].get_max()+offset-1)) ) wiped = FAILURE(1);
    }
  } else {
    if( scope[2].is_ground() ) {
      // it is already a precedence
      if( scope[2].get_min() ) {
	// x[0] + offset <= x[1]
	if(changed_idx) {
	  if(UB_CHANGED(evt) && IS_FAIL(scope[0].set_max(scope[1].get_max() - offset))) wiped = FAILURE(0);
	} else {
	  if(LB_CHANGED(evt) && IS_FAIL(scope[1].set_min(scope[0].get_min() + offset))) wiped = FAILURE(1);
	}
      } else {

	// std::cout << "APPLY " << scope[0] << " in " << scope[0].get_domain() << " + " << offset
	// 	  << " > "
	// 	  << scope[1] << " in " << scope[1].get_domain() << std::endl;

	// x[0] + offset > x[1]
	// x[1] + (1-k) <= x[0]	
	if(changed_idx) {
	  if(LB_CHANGED(evt) && IS_FAIL(scope[0].set_min(scope[1].get_min() - offset + 1))) wiped = FAILURE(0);
	} else {
	  if(UB_CHANGED(evt) && IS_FAIL(scope[1].set_max(scope[0].get_max() + offset - 1))) wiped = FAILURE(1);
	}
      }
    } else {
      // check if it must be a precedence
      if(changed_idx) {
	if(LB_CHANGED(evt)) {
	  // x[1] is too big to be less than or equal to  (x[0] + k - 1) ?
	  if(scope[1].get_min() > (scope[0].get_max() + offset -1) &&
	     IS_FAIL(scope[2].set_domain(1))) wiped = FAILURE(2); 
	}
	if(UB_CHANGED(evt)) {
	  // x[1] is too small to be greater than or equal to  (x[0] + k) ?
	  if(scope[1].get_max() < (scope[0].get_min() + offset) &&
	     IS_FAIL(scope[2].set_domain(0))) wiped = FAILURE(2); 
	}
      } else {
	if(LB_CHANGED(evt)) {
	  // x[0] is too big to be less than or equal to (x[1] - k) ?
	  if(scope[0].get_min() > (scope[1].get_max() - offset) &&
	     IS_FAIL(scope[2].set_domain(0))) wiped = FAILURE(2); 
	}  
      	if(UB_CHANGED(evt)) {
	  // x[0] is too small to be greater than or equal to (x[1] + 1 - k) ?
	  if(scope[0].get_max() < (scope[1].get_min() + 1 - offset) &&
	     IS_FAIL(scope[2].set_domain(1))) wiped = FAILURE(2); 
	}
      }
    }
  }

  return wiped;
}

std::ostream& Mistral::PredicateLess::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " <=> (" << scope[0]/*.get_var()*/ ;
  if(offset==0) os << " <= ";
  else if(offset==1) os << " < ";
  else os << " + " << offset << " <= ";
  os << scope[1]/*.get_var()*/ << ")";
  return os;
}

void Mistral::ConstraintLess::initialise() {
  ConstraintImplementation::initialise();
  //Constraint::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  //set_idempotent(true);
}

void Mistral::ConstraintLess::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}

Mistral::PropagationOutcome Mistral::ConstraintLess::propagate() {

//   std::cout << "changes: " << changes << std::endl;
//   std::cout << "events[0]: " << events[0] << " events[0]: " << events[1] << std::endl;

//   if(scope[0].id() == 6 && scope[1].id() == 9) {
//   std::cout << "propagate " << this << std::endl;
//   for(unsigned int i=0; i<scope.size; ++i)
//     std::cout << " " << scope[i].get_domain();
//   std::cout << std::endl;
//   }
  Mistral::PropagationOutcome wiped = CONSISTENT;
  if(scope[1].set_min(scope[0].get_min() + offset) == FAIL_EVENT) wiped = FAILURE(1);
  if(IS_OK(wiped) && scope[0].set_max(scope[1].get_max() - offset) == FAIL_EVENT) wiped = FAILURE(0);
  

//   if(scope[0].id() == 6 && scope[1].id() == 9) {
//     for(unsigned int i=0; i<scope.size; ++i)
//       std::cout << " " << scope[i].get_domain();
//     std::cout << std::endl;
//   }

  return wiped;
}

Mistral::PropagationOutcome Mistral::ConstraintLess::propagate(const int changed_idx, const Event evt) {

  Mistral::PropagationOutcome wiped = CONSISTENT;
  if(changed_idx==0) {
    if(LB_CHANGED(evt) && scope[1].set_min(scope[0].get_min() + offset) == FAIL_EVENT) wiped = FAILURE(1);
  } else {
    if(UB_CHANGED(evt) && scope[0].set_max(scope[1].get_max() - offset) == FAIL_EVENT) wiped = FAILURE(0);
  }

  return wiped;
}

std::ostream& Mistral::ConstraintLess::display(std::ostream& os) const {
  os << scope[0]/*.get_var()*/;
  if(offset < 0) os << " - " << (-offset+1) << " < ";
  else if(offset > 1) os << " + " << (offset-1) << " < ";
  else if(offset > 0) os << " < ";
  else os << " <= ";
  os << scope[1]/*.get_var()*/;
  return os;
}

Mistral::ConstraintDisjunctive::ConstraintDisjunctive(Variable x, Variable y, const int p0, const int p1) 
  : BinaryConstraint(x,y) { 
  processing_time[0] = p0; 
  processing_time[1] = p1;
  enforce_nfc1 = false;
}

Mistral::ConstraintDisjunctive::ConstraintDisjunctive(Vector< Variable >& scp, const int p0, const int p1) 
  : BinaryConstraint(scp) { 
  processing_time[0] = p0; 
  processing_time[1] = p1;
  enforce_nfc1 = false;
}

Mistral::ConstraintDisjunctive::ConstraintDisjunctive(std::vector< Variable >& scp, const int p0, const int p1) 
  : BinaryConstraint(scp) { 
  processing_time[0] = p0; 
  processing_time[1] = p1;
  enforce_nfc1 = false;
}

void Mistral::ConstraintDisjunctive::initialise() {
  
  precedence[0] = new ConstraintLess(scope[0], scope[1], processing_time[0]);
  precedence[1] = new ConstraintLess(scope[1], scope[0], processing_time[1]);

  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  //set_idempotent(true);
  //stress = 0;
}

void Mistral::ConstraintDisjunctive::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}

void Mistral::ConstraintDisjunctive::decide(const int choice) {

#ifdef _DEBUG_RELAX
      std::cout << "[" << std::setw(4) << id << "](" << name() << "): force relax" << std::endl;
#endif

  relax();
  //active = 0;

  if(choice==1) {

#ifdef _DEBUG_DISJUNCTIVE
    std::cout << "c add precedence " << precedence[1] << std::endl;
#endif

    get_solver()->add(precedence[1]);
  } else {

#ifdef _DEBUG_DISJUNCTIVE
    std::cout << "c add precedence " << precedence[0] << std::endl;
#endif

    get_solver()->add(precedence[0]);
  }

}

Mistral::PropagationOutcome Mistral::ConstraintDisjunctive::propagate() {
  int hold = 3;

  // check if prec[1] is violated (x1 + p1 > x0).
  if(scope[1].get_min()+processing_time[1] > scope[0].get_max()) {
    hold &= 2;
  }

  // check if prec[1] is violated (x0 + p0 > x1).
  if(scope[0].get_min()+processing_time[0] > scope[1].get_max()) {
    hold &= 1;
  }

  if(!hold) return FAILURE(0);
  if(hold<3) {
    decide(hold);
  }
  return CONSISTENT;
}

Mistral::PropagationOutcome Mistral::ConstraintDisjunctive::propagate(const int changed_idx, 
								      const Event evt) {
  int hold = 3;


  // if(id == 11) {
  //   std::cout << "propagate " ;
  //   display(std::cout);
  //   std::cout << " because of " << event2str(evt) << " on " << scope[changed_idx] << std::endl;
  // }

// if(id == 11) 
//   std::cout << scope[1].get_min() << " + " << processing_time[1] << " ?> " << scope[0].get_max() << std::endl;

  // check if prec[1] is violated (x1 + p1 > x0).
  //if(changed_idx && LB_CHANGED(evt))
  if(scope[1].get_min()+processing_time[1] > scope[0].get_max()) {
    hold &= 2;
  }

// if(id == 11) 
//   std::cout << scope[0].get_min() << " + " << processing_time[0] << " ?> " << scope[1].get_max() << std::endl;

  // check if prec[1] is violated (x0 + p0 > x1).
  if(scope[0].get_min()+processing_time[0] > scope[1].get_max()) {
    hold &= 1;
  }

// if(id == 11) {
//   std::cout << "HOLD: " << hold << std::endl;
//  }

  if(!hold) return FAILURE(0);
  if(hold<3) {
    decide(hold);
  }
  return CONSISTENT;
}

void Mistral::ConstraintDisjunctive::consolidate() {
  for(unsigned int i=0; i<2; ++i) {
    scope[i] = scope[i]/*.get_var()*/;
    _scope[i] = _scope[i]/*.get_var()*/;
    precedence[i].consolidate();
    // precedence[0].get_scope()[i] = scope[i];
    // precedence[1].get_scope([1-i] = scope[i];
  }
}

std::ostream& Mistral::ConstraintDisjunctive::display(std::ostream& os) const {
  os << precedence[0] << " or " 
     << precedence[1] ;
  return os;
}



// Mistral::ConstraintTernaryDisjunctive::ConstraintTernaryDisjunctive(Vector< Variable >& scp, const int p0, const int p1) : Constraint(scp) { 
//   processing_time[0] = p0; 
//   processing_time[1] = p1;
// }

// Mistral::ConstraintTernaryDisjunctive::ConstraintTernaryDisjunctive(std::vector< Variable >& scp, const int p0, const int p1) : Constraint(scp) { 
//   processing_time[0] = p0; 
//   processing_time[1] = p1;
// }

// void Mistral::ConstraintTernaryDisjunctive::initialise() {
  
//   precedence[0] = new ConstraintLess(scope, processing_time[0]);
//   precedence[1] = new ConstraintLess(processing_time[1]);
//   precedence[1]->add(scope[1]);
//   precedence[1]->add(scope[0]);

//   Constraint::initialise();
//   trigger_on(_RANGE_, scope[0]);
//   trigger_on(_RANGE_, scope[1]);
//   trigger_on(_VALUE_, scope[2]);
//   set_idempotent(true);
//   stress = 0;
// }

// Mistral::PropagationOutcome Mistral::ConstraintTernaryDisjunctive::decide(const int choice) {
//   PropagationOutcome wiped = CONSISTENT;

//   if(choice==1) {
//     if(scope[2].set_domain(1) == FAIL_EVENT) wiped = FAILURE(2);
//     else {
//       relax();
//       solver->add(precedence[1]);
//     }
//   } else {
//     if(scope[2].set_domain(0) == FAIL_EVENT) wiped = FAILURE(2);
//     else {
//       relax();
//       solver->add(precedence[0]);
//     }
//   }

//   return wiped;
// }

// void Mistral::ConstraintTernaryDisjunctive::decide() {
//   relax();

//   if(scope[2].get_min()) {
//     solver->add(precedence[1]);
//   } else {
//     solver->add(precedence[0]);
//   }

// }

// Mistral::PropagationOutcome Mistral::ConstraintTernaryDisjunctive::propagate() {
//   PropagationOutcome wiped = CONSISTENT;
//   int hold = 3;

//   if(events.contain(2)) {
    
//     decide();
 
//   } else {
    
//     // check if prec[1] is violated (x1 + p1 > x0).
//     if(scope[1].get_min()+processing_time[1] > scope[0].get_max()) {
//       hold &= 2;
//     }
    
//     // check if prec[1] is violated (x0 + p0 > x1).
//     if(scope[0].get_min()+processing_time[0] > scope[1].get_max()) {
//       hold &= 1;
//     }
    
//     if(!hold) return FAILURE(0);
//     if(hold<3) {
//       wiped = decide(hold);
//     }

//   }

//   return wiped;
// }

// void Mistral::ConstraintTernaryDisjunctive::consolidate() {
//   for(unsigned int i=0; i<2; ++i) {
//     scope[i] = scope[i]/*.get_var()*/;
//     precedence[0]->scope[i] = scope[i];
//     precedence[1]->scope[1-i] = scope[i];
//   }
//   scope[2] = scope[2]/*.get_var()*/;
// }

// std::ostream& Mistral::ConstraintTernaryDisjunctive::display(std::ostream& os) const {
//   os << precedence[0] << " or " 
//      << precedence[1] ;
//   return os;
// }

Mistral::ConstraintReifiedDisjunctive::ConstraintReifiedDisjunctive(Variable x, Variable y, Variable z, const int p0, const int p1) 
  : TernaryConstraint(x,y,z) { 
  processing_time[0] = p0; 
  processing_time[1] = p1;
}

Mistral::ConstraintReifiedDisjunctive::ConstraintReifiedDisjunctive(Vector< Variable >& scp, const int p0, const int p1) 
  : TernaryConstraint(scp) { 
  processing_time[0] = p0; 
  processing_time[1] = p1;
}

Mistral::ConstraintReifiedDisjunctive::ConstraintReifiedDisjunctive(std::vector< Variable >& scp, const int p0, const int p1) 
  : TernaryConstraint(scp) { 
  processing_time[0] = p0; 
  processing_time[1] = p1;

}

void Mistral::ConstraintReifiedDisjunctive::initialise() {

  ConstraintImplementation::initialise();

  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  trigger_on(_VALUE_, scope[2]);
  //set_idempotent(true);
  //stress = 0;

  state = scope[2].get_var().bool_domain;

  if(scope[0].domain_type == RANGE_VAR) {
    min_t0_ptr = &(scope[0].range_domain->min);
    max_t0_ptr = &(scope[0].range_domain->max);
  } else if(scope[0].domain_type == BITSET_VAR) {
    min_t0_ptr = &(scope[0].bitset_domain->domain.min);
    max_t0_ptr = &(scope[0].bitset_domain->domain.max);
  } else {
    min_t0_ptr = &(scope[0].get_var().range_domain->min);
    max_t0_ptr = &(scope[0].get_var().range_domain->max);
    //std::cerr << "TODO " << scope[0].domain_type << std::endl;
    //exit(1);
  }

  if(scope[1].domain_type == RANGE_VAR) {
    min_t1_ptr = &(scope[1].range_domain->min);
    max_t1_ptr = &(scope[1].range_domain->max);
  } else if(scope[1].domain_type == BITSET_VAR) {
    min_t1_ptr = &(scope[1].bitset_domain->domain.min);
    max_t1_ptr = &(scope[1].bitset_domain->domain.max);
  } else {
    min_t1_ptr = &(scope[1].get_var().range_domain->min);
    max_t1_ptr = &(scope[1].get_var().range_domain->max);
    //std::cerr << "TODO " << scope[1].domain_type << std::endl;
    //exit(1);
  }

}

void Mistral::ConstraintReifiedDisjunctive::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}

Mistral::PropagationOutcome Mistral::ConstraintReifiedDisjunctive::propagate() {
  return CONSISTENT;
}



Mistral::PropagationOutcome Mistral::ConstraintReifiedDisjunctive::propagate(const int changed_idx, const Event evt) {
  PropagationOutcome wiped = CONSISTENT;


#ifdef _DEBUG_RDISJUNCTIVE
  std::cout << std::endl << "propagate [" << id << "] " << this << " " << event2str(evt) 
	    << " on " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << std::endl;
  for(unsigned int i=0; i<3; ++i) {
    std::cout << scope[i] << " in " << scope[i].get_domain() << " ";
  }
  std::cout << std::endl;
#endif


  if(changed_idx == 2) {

#ifdef _DEBUG_RDISJUNCTIVE
    std::cout << "  -> now it's a precedence" << std::endl;
#endif
    
    // it becomes a precedence, we must enforce both rules no matter what and there cannot be further changes.
    
    if(LB_CHANGED(evt)) {
      if( scope[0].set_max( *max_t1_ptr-processing_time[0] ) == FAIL_EVENT) wiped = FAILURE(0);
      else if( scope[1].set_min( *min_t0_ptr+processing_time[0] ) == FAIL_EVENT) wiped = FAILURE(1);
    } else {
      if( scope[0].set_min( *min_t1_ptr+processing_time[1] ) == FAIL_EVENT) wiped = FAILURE(0);
      else if( scope[1].set_max( *max_t0_ptr-processing_time[1] ) == FAIL_EVENT) wiped = FAILURE(1);
    }
  } else if(*state!=3) {
    

#ifdef _DEBUG_RDISJUNCTIVE
    std::cout << "  -> it was already a precedence" << std::endl;
#endif

    // it was already a precedence, we enforce the changes

    if(*state==2) {
      if(// events.contain(1) 
	 changed_idx == 1
	 && UB_CHANGED(evt)) {
	if( scope[0].set_max( *max_t1_ptr-processing_time[0] ) == FAIL_EVENT) wiped = FAILURE(0);
      }
      if(IS_OK(wiped) && // events.contain(0)
	 changed_idx == 0
	 && LB_CHANGED(evt)) {
	if( scope[1].set_min( *min_t0_ptr+processing_time[0] ) == FAIL_EVENT) wiped = FAILURE(1);
      }
    } else {
      if(// events.contain(0)
	 changed_idx == 0
	 && UB_CHANGED(evt)) {
	if( scope[1].set_max( *max_t0_ptr-processing_time[1] ) == FAIL_EVENT) wiped = FAILURE(1);
      }
      if(IS_OK(wiped) && // events.contain(1)
	 changed_idx == 1
	 && LB_CHANGED(evt)) {
	if( scope[0].set_min( *min_t1_ptr+processing_time[1] ) == FAIL_EVENT) wiped = FAILURE(0);
      }
    } 
  } else {

#ifdef _DEBUG_RDISJUNCTIVE
    std::cout << "  -> could it be a precedence? (" 
	      << *min_t0_ptr << " + " << processing_time[0] 
	      << " > " <<  *max_t1_ptr << " || "
	      << *min_t1_ptr << " + " << processing_time[1] 
	      << " > " << *max_t0_ptr << ")" << std::endl;
#endif
    
    // the disjunction is not yet decided, we check if it should

    if( *min_t0_ptr + processing_time[0] > *max_t1_ptr ) {

      if( scope[2].set_domain(0) == FAIL_EVENT) wiped = FAILURE(2);
      // x[1]+p[1] <= x[0] because x[0]'s min is too high or x[1]'s max is too low
      else if( scope[0].set_min( *min_t1_ptr+processing_time[1] ) == FAIL_EVENT) wiped = FAILURE(0);
      else if( scope[1].set_max( *max_t0_ptr-processing_time[1] ) == FAIL_EVENT) wiped = FAILURE(1);

#ifdef _DEBUG_RDISJUNCTIVE
      std::cout << "  -> YES!: " ;
      display(std::cout);
      std::cout << std::endl;
#endif

    } else if( *min_t1_ptr + processing_time[1] > *max_t0_ptr ) {

      if( scope[2].set_domain(1) == FAIL_EVENT) wiped = FAILURE(2);
      else if( scope[0].set_max( *max_t1_ptr-processing_time[0] ) == FAIL_EVENT) wiped = FAILURE(0);
      else if( scope[1].set_min( *min_t0_ptr+processing_time[0] ) == FAIL_EVENT) wiped = FAILURE(1);
    
#ifdef _DEBUG_RDISJUNCTIVE
      std::cout << "  -> YES!: " ;
      display(std::cout);
      std::cout << std::endl;
#endif

    } 
#ifdef _DEBUG_RDISJUNCTIVE
    else 
      std::cout << "  -> NO!" << std::endl;
#endif

  }

#ifdef _DEBUG_RDISJUNCTIVE
  for(unsigned int i=0; i<3; ++i) {
    std::cout << scope[i] << " in " << scope[i].get_domain() << " ";
  }
  std::cout << std::endl;
  std::cout << "+-> " << (IS_OK(wiped) ? "OK" : "inconsistency!") << std::endl;
#endif
  
  //changes.clear();

  return wiped;
}

std::ostream& Mistral::ConstraintReifiedDisjunctive::display(std::ostream& os) const {
  if(scope[2].is_ground()) {
    if(!scope[2].get_max()) {
      os << scope[1] << " + " << processing_time[1] << " <= " << scope[0]/*.get_var()*/ ;
    } else {
      os << scope[0] << " + " << processing_time[0] << " <= " << scope[1]/*.get_var()*/ ;
    }
  } else {
    os << scope[0]/*.get_var()*/ << " + " << processing_time[0] << " <= " << scope[1]/*.get_var()*/ << " or " 
       << scope[1]/*.get_var()*/ << " + " << processing_time[1] << " <= " << scope[0]/*.get_var()*/ ;
  }
  return os;
}


void Mistral::PredicateEqual::initialise() {
    ConstraintImplementation::initialise();

  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_DOMAIN_, scope[1]);
  trigger_on(_VALUE_, scope[2]);

  //ConstraintImplementation::initialise();
  //set_idempotent(false);
}

void Mistral::PredicateEqual::mark_domain() {
  get_solver()->forbid(scope[0].id(), RANGE_VAR|LIST_VAR);
  get_solver()->forbid(scope[1].id(), RANGE_VAR|LIST_VAR);
}

Mistral::PropagationOutcome Mistral::PredicateEqual::rewrite() {

  // if(id==49) {
  //   std::cout << is_posted << " rewrite (" << scope[0] << " in " << scope[0].get_domain()
  // 	      << " == " << scope[1] << " in " << scope[1].get_domain()
  // 	      << ") <-> " << scope[2] << " in " << scope[2].get_domain() << std::endl;
  // }

  Mistral::PropagationOutcome wiped = propagate();

#ifdef _COMPLETE
  if( active.size == 2 ) {
    relax();

    if( scope[2].is_ground() ) {
      if( (spin + scope[2].get_min()) == 1 ) {
	solver->add(new ConstraintNotEqual(scope));
      } else {

	// std::cout << "   => change to an equal constraint" << std::endl;

	--scope.size;
	solver->add(new ConstraintEqual(scope));
      }
    } else {
      Vector< Variable > new_scope;

      int val = NOVAL, ground_idx = scope[1].is_ground();
      new_scope.add(scope[1-ground_idx]);
      new_scope.add(scope[2]);
      
      val = scope[ground_idx].get_min();
      solver->add(new PredicateConstantEqual(new_scope, val, spin));
    }
  }
#endif

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateEqual::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[2].is_ground() ) {
    if( (spin + scope[2].get_min()) == 1 ) {
      if(scope[0].is_ground() && IS_FAIL(scope[1].remove(scope[0].get_min())))
	wiped = FAILURE(1);
      else {
	if(scope[1].is_ground() && IS_FAIL(scope[0].remove(scope[1].get_min())))
	  wiped = FAILURE(0);
      }
    } else {
      if( IS_FAIL(scope[0].set_domain(scope[1])) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_domain(scope[0])) ) wiped = FAILURE(1);
    }
  } else {
    if( !(scope[0].intersect(scope[1])) ) {
      if( IS_FAIL(scope[2].remove(spin)) ) wiped = FAILURE(2);	    
    } else { 
      if( scope[0].is_ground() && scope[1].is_ground() ) {
	if( IS_FAIL(scope[2].set_domain(spin) )) wiped = FAILURE(2);
      }
    }
  }
  
  return wiped;
}


Mistral::PropagationOutcome Mistral::PredicateEqual::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( changed_idx == 2 ) {
    if( LB_CHANGED(evt) == spin ) {
      // x[0] == x[1]
      if( IS_FAIL(scope[0].set_domain(scope[1])) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_domain(scope[0])) ) wiped = FAILURE(1);
    } else {
      // x[0] != x[1]
      if(scope[0].is_ground() && IS_FAIL(scope[1].remove(scope[0].get_min()))) wiped = FAILURE(1);
      else if(scope[1].is_ground() && IS_FAIL(scope[0].remove(scope[1].get_min()))) wiped = FAILURE(0);
    }
  } else {
    if( scope[2].is_ground() ) {

      // std::cout << "HERE: " << spin << " " <<  scope[2].get_min() << " " << scope[changed_idx].get_domain() << std::endl;

      if( (spin != scope[2].get_min()) ) {
	if(scope[changed_idx].is_ground() && IS_FAIL(scope[1-changed_idx].remove(scope[changed_idx].get_min()))) 
	  wiped = FAILURE(1-changed_idx);
      } else {
	if( IS_FAIL(scope[1-changed_idx].set_domain(scope[changed_idx])) ) 
	  wiped = FAILURE(1-changed_idx);
      }
    } else {
      // check if (in)equality can be deduced
      if( !(scope[0].intersect(scope[1])) ) {
	if( IS_FAIL(scope[2].remove(spin)) ) wiped = FAILURE(2);	    
      } else { 
	if( scope[0].is_ground() && scope[1].is_ground() ) {
	  if( IS_FAIL(scope[2].set_domain(spin) )) wiped = FAILURE(2);
	}
      }
    }
  }
  
  return wiped;
}

std::ostream& Mistral::PredicateEqual::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " <=> (";
  if(spin) os << scope[0]/*.get_var()*/ << " == " << scope[1]/*.get_var()*/;
  else os << scope[0]/*.get_var()*/ << " =/= " << scope[1]/*.get_var()*/;
  os << ")";
  return os;
}


void Mistral::PredicateConstantEqual::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_VALUE_, scope[1]);
}

void Mistral::PredicateConstantEqual::mark_domain() {
  get_solver()->forbid(scope[0].id(), RANGE_VAR);
  //get_solver()->mark_non_convex(scope[0].id());
}

Mistral::PropagationOutcome Mistral::PredicateConstantEqual::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].is_ground() ) {
    if( (spin + scope[1].get_min()) == 1 ) {
      if( IS_FAIL(scope[0].remove(value)) ) wiped = FAILURE(0);
    } else {
      if( IS_FAIL(scope[0].set_domain(value)) ) wiped = FAILURE(0);
    }
  } else {
    if( !(scope[0].contain(value)) ) {
      if( IS_FAIL(scope[1].remove(spin)) ) wiped = FAILURE(1);	    
    } else if( scope[0].is_ground() ) {
      if( IS_FAIL(scope[1].set_domain(spin)) ) wiped = FAILURE(1);
    }
  }
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateConstantEqual::propagate(const int changed_idx, const Event evt) {
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( changed_idx ) {
    if( (spin + LB_CHANGED(evt)) == 1 ) {
      if( IS_FAIL(scope[0].remove(value)) ) wiped = FAILURE(0);
    } else {
      if( IS_FAIL(scope[0].set_domain(value)) ) wiped = FAILURE(0);
    }
  } else {
    if( !(scope[0].contain(value)) ) {
      if( IS_FAIL(scope[1].remove(spin)) ) wiped = FAILURE(1);	    
    } else if( scope[0].is_ground() ) {
      if( IS_FAIL(scope[1].set_domain(spin)) ) wiped = FAILURE(1);
    }
  }

  return wiped;
}

std::ostream& Mistral::PredicateConstantEqual::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " <=> (";
  if(spin) os << scope[0]/*.get_var()*/ << " == " << value;
  else os << scope[0]/*.get_var()*/ << " =/= " << value;
  os << ")";
  return os;
}


void Mistral::PredicateIntervalMember::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_VALUE_, scope[1]);
}

void Mistral::PredicateIntervalMember::mark_domain() {
  get_solver()->forbid(scope[0].id(), RANGE_VAR|LIST_VAR);
  //get_solver()->mark_non_convex(scope[0].id());
}

Mistral::PropagationOutcome Mistral::PredicateIntervalMember::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].is_ground() ) {
    if( (spin + scope[1].get_min()) == 1 ) {
      if( IS_FAIL(scope[0].remove_interval(lower_bound, upper_bound)) ) wiped = FAILURE(0);
    } else {
      //if( IS_FAIL(scope[0].set_domain(lower_bound, upper_bound)) ) wiped = FAILURE(0);
      if( IS_FAIL(scope[0].set_min(lower_bound)) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[0].set_max(upper_bound)) ) wiped = FAILURE(0);
    }
  } else {

    //std::cout << scope[0].get_domain() << " <> [" << lower_bound << ".." << upper_bound << "]" << std::endl; 

    if( !(scope[0].intersect(lower_bound, upper_bound)) ) {
      if( IS_FAIL(scope[1].remove(spin)) ) wiped = FAILURE(1);	    
    } else if( scope[0].included(lower_bound, upper_bound) ) {
      if( IS_FAIL(scope[1].set_domain(spin)) ) wiped = FAILURE(1);
    }
  }
  
  //exit(1);

  return wiped;
}


Mistral::PropagationOutcome Mistral::PredicateIntervalMember::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if(changed_idx == 1) {
    if( (spin + LB_CHANGED(evt)) == 1 ) {
      if( IS_FAIL(scope[0].remove_interval(lower_bound, upper_bound)) ) wiped = FAILURE(0);
    } else {
      if( IS_FAIL(scope[0].set_min(lower_bound)) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[0].set_max(upper_bound)) ) wiped = FAILURE(0);
    }
  } else {
    if( !(scope[0].intersect(lower_bound, upper_bound)) ) {
      if( IS_FAIL(scope[1].remove(spin)) ) wiped = FAILURE(1);	    
    } else if( scope[0].included(lower_bound, upper_bound) ) {
      if( IS_FAIL(scope[1].set_domain(spin)) ) wiped = FAILURE(1);
    }
  }

  return wiped;
}

std::ostream& Mistral::PredicateIntervalMember::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " <=> (";
  if(spin) os << scope[0]/*.get_var()*/ << " in [" << lower_bound << ".." << upper_bound << "]";
  else os << scope[0]/*.get_var()*/ << " in ..]" << lower_bound << "," << upper_bound << "[..";
  os << ")";
  return os;
}


void Mistral::PredicateSetMember::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_VALUE_, scope[1]);
  //set_idempotent(false);

  non_values.initialise(scope[0].get_min(), scope[0].get_max(), BitSet::full);
  non_values.setminus_with(values);
}

void Mistral::PredicateSetMember::mark_domain() {
  get_solver()->forbid(scope[0].id(), RANGE_VAR|LIST_VAR);
  //get_solver()->mark_non_convex(scope[0].id());
}

Mistral::PropagationOutcome Mistral::PredicateSetMember::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].is_ground() ) {
    if( (spin + scope[1].get_min()) == 1 ) {
      if( IS_FAIL(scope[0].set_domain(non_values)) ) wiped = FAILURE(0);
    } else {
      if( IS_FAIL(scope[0].set_domain(values)) ) wiped = FAILURE(0);
    }
  } else {
    if( !(scope[0].intersect(values)) ) {
      if( IS_FAIL(scope[1].remove(spin)) ) wiped = FAILURE(1);	    
    } else if( scope[0].included(values) ) {
      if( IS_FAIL(scope[1].set_domain(spin)) ) wiped = FAILURE(1);
    }
  }
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateSetMember::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( changed_idx == 1 ) {
    if( (spin + LB_CHANGED(evt)) == 1 ) {
      if( IS_FAIL(scope[0].set_domain(non_values)) ) wiped = FAILURE(0);
    } else {
      if( IS_FAIL(scope[0].set_domain(values)) ) wiped = FAILURE(0);
    }
  } else {
    if( !(scope[0].intersect(values)) ) {
      if( IS_FAIL(scope[1].remove(spin)) ) wiped = FAILURE(1);	    
    } else if( scope[0].included(values) ) {
      if( IS_FAIL(scope[1].set_domain(spin)) ) wiped = FAILURE(1);
    }
  }
  
  return wiped;
}

std::ostream& Mistral::PredicateSetMember::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " <=> (" << scope[0]/*.get_var()*/ ;
  if(!spin) os << " not";
  os << " in " << values; 
  os << ")";
  return os;
}


void Mistral::PredicateOffset::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
}

void Mistral::PredicateOffset::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}

Mistral::PropagationOutcome Mistral::PredicateOffset::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;


#ifdef _DEBUG_OFFSET
  if(_DEBUG_OFFSET) {
    std::cout << scope[0].get_domain() << " + " << offset << " = " << scope[1].get_domain() << std::endl;
  }
#endif


  if( IS_FAIL(scope[0].set_min( scope[1].get_min() - offset )) ) wiped = FAILURE(0); 
  else if( IS_FAIL(scope[0].set_max( scope[1].get_max() - offset )) ) wiped = FAILURE(0); 
  else if( IS_FAIL(scope[1].set_min( scope[0].get_min() + offset )) ) wiped = FAILURE(1); 
  else if( IS_FAIL(scope[1].set_max( scope[0].get_max() + offset )) ) wiped = FAILURE(1);


#ifdef _DEBUG_OFFSET
  if(_DEBUG_OFFSET) {
    if(IS_OK(wiped))
      std::cout << scope[0].get_domain() << " + " << offset << " = " << scope[1].get_domain() ;
    else
      std::cout << "fail!" ;
    std::cout << std::endl << std::endl;
  }
#endif

  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateOffset::propagate(const int changed_idx, const Event evt) {
  Mistral::PropagationOutcome wiped = CONSISTENT;


#ifdef _DEBUG_OFFSET
  if(_DEBUG_OFFSET) {
    std::cout << scope[0].get_domain() << " + " << offset << " = " << scope[1].get_domain() << std::endl;
  }
#endif

  int changed = changed_idx;
  Event enxt, ecur=evt;
  while(IS_OK(wiped) && ecur != NO_EVENT) {
    enxt = NO_EVENT;
    
#ifdef _DEBUG_OFFSET
  if(_DEBUG_OFFSET) {
    std::cout << "react to " << event2str(ecur) << " on " << scope[changed] << std::endl;
  }
#endif

    if(changed == 1) {
      if( LB_CHANGED(ecur) ) {
	enxt |= scope[0].set_min( scope[1].get_min() - offset );
	if(FAILED(enxt)) wiped = FAILURE(0); 
      }
      if( IS_OK(wiped) && UB_CHANGED(ecur) ) {
	enxt |= scope[0].set_max( scope[1].get_max() - offset );
	if(FAILED(enxt)) wiped = FAILURE(0);
      }
      changed = 0;
    } else { 
      if( LB_CHANGED(ecur) ) {
	enxt |= scope[1].set_min( scope[0].get_min() + offset );
        if(FAILED(enxt)) wiped = FAILURE(1);
      } 
      if( IS_OK(wiped) && UB_CHANGED(ecur) ) {
	enxt |= scope[1].set_max( scope[0].get_max() + offset );
	if(FAILED(enxt)) wiped = FAILURE(1);
      }
      changed = 1;
    }

#ifdef _DEBUG_OFFSET
  if(_DEBUG_OFFSET) {
    std::cout << " ====> " << event2str(enxt) << " on " << scope[changed] << std::endl;
  }
#endif

    ecur = enxt;
  }


#ifdef _DEBUG_OFFSET
  if(_DEBUG_OFFSET) {
    if(IS_OK(wiped))
      std::cout << scope[0].get_domain() << " + " << offset << " = " << scope[1].get_domain() ;
    else
      std::cout << "fail!" ;
    std::cout << std::endl << std::endl;
  }
#endif


  return wiped;
}

std::ostream& Mistral::PredicateOffset::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " + " << offset << ")";
  return os;
}


void Mistral::PredicateFactor::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
}

void Mistral::PredicateFactor::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}


Mistral::PropagationOutcome Mistral::PredicateFactor::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( IS_FAIL(scope[0].set_min( (factor>0 ? 
				 scope[1].get_min() : 
				 scope[1].get_max()) / factor )) ) {
    wiped = FAILURE(0); 

  } else if( IS_FAIL(scope[0].set_max( (factor>0 ? 
				      scope[1].get_max() :
					scope[1].get_min()) / factor )) ) {
    wiped = FAILURE(0); 

  } else if( IS_FAIL(scope[1].set_min( (factor>0 ? 
					scope[0].get_min() : 
					scope[0].get_max()) * factor )) ) {
    wiped = FAILURE(1); 

  } else if( IS_FAIL(scope[1].set_max( (factor>0 ? 
					scope[0].get_max() :
					scope[0].get_min()) * factor )) ) {
    wiped = FAILURE(1);

  }

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateFactor::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if(changed_idx == 1) {
    if(factor>0) {
      if( LB_CHANGED(evt) && IS_FAIL(scope[0].set_min( scope[1].get_min() / factor )) )
	wiped = FAILURE(0); 
      if( UB_CHANGED(evt) && IS_FAIL(scope[0].set_max( scope[1].get_max() / factor )) ) 
	wiped = FAILURE(0); 
    } else {
      if( UB_CHANGED(evt) && IS_FAIL(scope[0].set_min( scope[1].get_max() / factor )) )
	wiped = FAILURE(0); 
      if( LB_CHANGED(evt) && IS_FAIL(scope[0].set_max( scope[1].get_min() / factor )) )
	wiped = FAILURE(0); 
    }
  } else {
    if(factor>0) {
      if( LB_CHANGED(evt) && IS_FAIL(scope[1].set_min( scope[0].get_min() * factor )) )
	wiped = FAILURE(1); 
      if( UB_CHANGED(evt) && IS_FAIL(scope[1].set_max( scope[0].get_max() * factor )) ) 
	wiped = FAILURE(1); 
    } else {
      if( UB_CHANGED(evt) && IS_FAIL(scope[1].set_min( scope[0].get_max() * factor )) )
	wiped = FAILURE(1); 
      if( LB_CHANGED(evt) && IS_FAIL(scope[1].set_max( scope[0].get_min() * factor )) )
	wiped = FAILURE(1); 
    }
  }
  
  return wiped;
}

std::ostream& Mistral::PredicateFactor::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " * " << factor << ")";
  return os;
}



void Mistral::PredicateAbs::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_DOMAIN_, scope[1]);

  int lb = -scope[1].get_max();
  if(lb < scope[0].get_min())
    lb = scope[0].get_min();
  int ub = scope[1].get_max();
  if(ub > scope[0].get_max())
    ub = scope[0].get_max();
  
  buffer.initialise(lb, ub, BitSet::empt);
}

void Mistral::PredicateAbs::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}


Mistral::PropagationOutcome Mistral::PredicateAbs::propagate_change_on_X(const Event evt) {
  //if(DOMAIN_CHANGED(evt))
  Mistral::PropagationOutcome wiped = CONSISTENT;

#ifdef _DEBUG_ABS
  if(_DEBUG_ABS) {
    std::cout << "*abs(" << scope[0].get_domain() << ") = " << scope[1].get_domain() << std::endl;
  }
#endif

  if(scope[0].is_range()) {
    int lb = 0;
    if(scope[0].get_max() < 0) lb = -scope[0].get_max();
    if(scope[0].get_min() > 0) lb =  scope[0].get_min();

    if(IS_FAIL(scope[1].set_domain(
				   lb,
				   std::max(
					    scope[0].get_max(),
					    -scope[0].get_min())
				   ))) wiped = FAILURE(1);
  } else {
    scope[0].put_negation_in(buffer);
    scope[0].union_to(buffer);
    
    if(IS_FAIL(scope[1].set_domain(buffer))) wiped = FAILURE(1);
  }

#ifdef _DEBUG_ABS
  if(_DEBUG_ABS) {
    std::cout << "*abs(" << scope[0].get_domain() << ") = " << scope[1].get_domain() << std::endl << std::endl;
  }
#endif
  
  return wiped;
}


Mistral::PropagationOutcome Mistral::PredicateAbs::propagate_change_on_absX(const Event evt) {
  //if(DOMAIN_CHANGED(evt))
  Mistral::PropagationOutcome wiped = CONSISTENT;

#ifdef _DEBUG_ABS
  if(_DEBUG_ABS) {
    std::cout << "abs(" << scope[0].get_domain() << ") = *" << scope[1].get_domain() << std::endl;
  }
#endif


  if(scope[1].is_range()) {
    if(IS_FAIL(scope[0].set_domain(-scope[1].get_max(), scope[1].get_max()))) wiped = FAILURE(0);
    else {
      
      // std::cout << "scope[1].get_min() > 0 : " << (scope[1].get_min() > 0) << std::endl;
      // std::cout << "[" << (1-scope[1].get_min()) << "," << (scope[1].get_min()-1) << "]\n";

      if(scope[1].get_min() > 0 && IS_FAIL(scope[0].remove_interval(1-scope[1].get_min(), scope[1].get_min()-1))) wiped = FAILURE(0);

    }
  } else {
    scope[1].put_negation_in(buffer);
    scope[1].union_to(buffer);
    
    if(IS_FAIL(scope[0].set_domain(buffer))) wiped = FAILURE(0);
  }


#ifdef _DEBUG_ABS
  if(_DEBUG_ABS) {
    std::cout << "abs(" << scope[0].get_domain() << ") = *" << scope[1].get_domain() << std::endl << std::endl;
  }
#endif

  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateAbs::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  wiped = propagate_change_on_X( LB_EVENT|UB_EVENT );
  if(IS_OK(wiped)) wiped = propagate_change_on_absX( LB_EVENT|UB_EVENT );

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateAbs::propagate(const int changed_idx, const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if(changed_idx) wiped = propagate_change_on_absX( evt );
  else wiped = propagate_change_on_X( evt );
  
  return wiped;
}

std::ostream& Mistral::PredicateAbs::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " == abs(" << scope[0]/*.get_var()*/ << ")";
  return os;
}


void Mistral::PredicateNot::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_VALUE_, scope[0]);
  trigger_on(_VALUE_, scope[1]);

}


Mistral::PropagationOutcome Mistral::PredicateNot::rewrite() {
  RewritingOutcome r_evt = NO_EVENT; 

  if(scope[0].is_ground() || scope[1].is_ground() ) {

#ifdef _DEBUG_REWRITE
    std::cout << "    relax ";
    display(std::cout);
    std::cout << std::endl;
#endif

    r_evt = SUPPRESSED;
    relax();
  } else {
    
    // find out if the constraints on x[0] can be negated

    bool hold = true;
    for(int var=0; hold && var<2; ++var) {

      int idx = scope[var].id();
      bool can_negate = true;
      Constraint con;
      for(Event trig = 0; can_negate && trig<3; ++trig) {
	for(int i = get_solver()->constraint_graph[idx].on[trig].size; can_negate && i--;) {
	  con = get_solver()->constraint_graph[idx].on[trig][i];

	  // std::cout << con << (con.absorb_negation(con.index()) ? " can" : " cammot") 
	  // 	    << " absorb negation of " << con.get_scope()[con.index()] << std::endl;

	  can_negate &= con.absorb_negation(con.index());
	}
      }
      
      if(can_negate) {
	
	hold = false;

	r_evt = SUPPRESSED;
	
#ifdef _DEBUG_REWRITE
	std::cout << "    relax ";
	display(std::cout);
	std::cout << " and remove " << scope[var] << std::endl;
#endif
	
	relax();
	get_solver()->remove(scope[var]);
	
	for(Event trig = 0; trig<3; ++trig) {
	  for(int i = get_solver()->constraint_graph[idx].on[trig].size; i--;) {
	    con = get_solver()->constraint_graph[idx].on[trig][i];
	    
#ifdef _DEBUG_REWRITE
	    std::cout << "      relax " << con << std::endl;
#endif
	    
	    con.relax();
	    con = Constraint(con.get_negation(con.index(), scope[1-var]));

#ifdef _DEBUG_REWRITE
	    std::cout << "      post " << con << std::endl;
#endif

	    get_solver()->add( con );
	  }
	}
      }
    }
  }  

  return r_evt;
}


Mistral::PropagationOutcome Mistral::PredicateNot::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].is_ground() ) {
    if( scope[1].get_min() ) { // x[0] == 0
      if( IS_FAIL(scope[0].set_domain(0)) ) wiped = FAILURE(0);
    } else if( scope[1].get_max() == 0 ) { //x[0] != 0
      if( IS_FAIL(scope[0].remove(0)) ) wiped = FAILURE(0);
    } 
  } else {
    if( !scope[0].contain(0) ) {
      if(IS_FAIL(scope[1].set_domain(0))) wiped = FAILURE(1);
    } else if( scope[0].is_ground() ) {
      if(IS_FAIL(scope[1].remove(0))) wiped = FAILURE(1);
    }
  }
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateNot::propagate(const int changed_idx, const Event evt) {
  Mistral::PropagationOutcome wiped = CONSISTENT;

  //  std::cout << std::endl << scope[0].get_domain() << " " << scope[1].get_domain() << std::endl; 

  // if( changed_idx ) {
  //   if( LB_CHANGED(evt) ) { // x[0] == 0
  //     if( IS_FAIL(scope[0].set_domain(0)) ) wiped = FAILURE(0);
  //   } else { //x[0] != 0
  //     if( IS_FAIL(scope[0].remove(0)) ) wiped = FAILURE(0);
  //   } 
  // } else {
  //   if( LB_CHANGED(evt) ) {
  //     if(IS_FAIL(scope[1].set_domain(0))) wiped = FAILURE(1);
  //   } else {
  //     if(IS_FAIL(scope[1].remove(0))) wiped = FAILURE(1);
  //   }
  // }

  if( IS_FAIL( scope[1-changed_idx].set_domain(UB_CHANGED(evt)) ) ) wiped = FAILURE(1-changed_idx);


  //  std::cout << scope[0].get_domain() << " " << scope[1].get_domain() << std::endl << std::endl ; 
  
  return wiped;
}

std::ostream& Mistral::PredicateNot::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " <=> not(" << scope[0]/*.get_var()*/ << ")";
  return os;
}


void Mistral::PredicateNeg::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_DOMAIN_, scope[1]);
}

void Mistral::PredicateNeg::mark_domain() {
  get_solver()->forbid(scope[0].id(), LIST_VAR);
  get_solver()->forbid(scope[1].id(), LIST_VAR);
}


Mistral::PropagationOutcome Mistral::PredicateNeg::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  for(int changed_idx = 0; changed_idx < 2; ++changed_idx) {
    if(IS_FAIL(scope[1-changed_idx].set_max(-scope[changed_idx].get_min()))) wiped = FAILURE(1-changed_idx);
    if(IS_FAIL(scope[1-changed_idx].set_min(-scope[changed_idx].get_max()))) wiped = FAILURE(1-changed_idx);
    
    if( scope[changed_idx].get_size() < scope[1-changed_idx].get_size() ) {
      int vali, vnxt = scope[1-changed_idx].get_min();
      do {
	vali = vnxt;
	if(!scope[changed_idx].contain(-vali) && IS_FAIL(scope[1-changed_idx].remove(vali))) wiped = FAILURE(1-changed_idx);
	vnxt = scope[1-changed_idx].next(vali);
      } while(vali < vnxt);
    }
  }

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateNeg::propagate(const int changed_idx, const Event evt) {
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( ASSIGNED(evt) ) {
    if(IS_FAIL(scope[1-changed_idx].set_domain(-scope[changed_idx].get_min()))) wiped = FAILURE(1-changed_idx);
  } else {
    if( LB_CHANGED(evt) ) {
      if(IS_FAIL(scope[1-changed_idx].set_max(-scope[changed_idx].get_min()))) wiped = FAILURE(1-changed_idx);
    }
    if( UB_CHANGED(evt) ) {
      if(IS_FAIL(scope[1-changed_idx].set_min(-scope[changed_idx].get_max()))) wiped = FAILURE(1-changed_idx);
    }
    if( scope[changed_idx].get_size() < scope[1-changed_idx].get_size() ) {
      int vali, vnxt = scope[1-changed_idx].get_min();
      do {
	vali = vnxt;
	if(!scope[changed_idx].contain(-vali) && IS_FAIL(scope[1-changed_idx].remove(vali))) wiped = FAILURE(1-changed_idx);
	vnxt = scope[1-changed_idx].next(vali);
      } while(vali < vnxt);
    }
  }
  
  return wiped;
}

std::ostream& Mistral::PredicateNeg::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " = -(" << scope[0]/*.get_var()*/ << ")";
  return os;
}


void Mistral::PredicateAnd::initialise() {
  ConstraintImplementation::initialise();
  for(int i=0; i<3; ++i)
    trigger_on(_VALUE_, scope[i]);
}

Mistral::PropagationOutcome Mistral::PredicateAnd::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  // std::cout << std::endl << "PROPAGATE " << this << std::endl;
  // std::cout << scope[0].get_domain() << " & " 
  // 	    << scope[1].get_domain() << " <-> "
  // 	    << scope[2].get_domain() << std::endl; 


  if( scope[2].is_ground() ) {
    if( scope[2].get_min() ) {
      if( IS_FAIL(scope[0].remove(0)) ) 
	wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].remove(0)) ) 
	wiped = FAILURE(1);
    } else if( scope[2].get_max() == 0 ) {
      if( scope[1].get_min() ){
	if( IS_FAIL(scope[0].set_domain(0)) ) wiped = FAILURE(0);
      } else if( scope[0].get_min() ) {
	if( IS_FAIL(scope[1].set_domain(0)) ) wiped = FAILURE(1);
      }
    } 
  } else {
    if( scope[0].get_min() && scope[1].get_min() ) {
      if( IS_FAIL(scope[2].remove(0)) ) return FAILURE(2);
    } else if( !scope[0].get_max() || !scope[1].get_max() ) {
      if( IS_FAIL(scope[2].set_domain(0)) ) return FAILURE(2);
    }
  }

 // std::cout << scope[0].get_domain() << " & " 
 // 	    << scope[1].get_domain() << " <-> "
 // 	   << scope[2].get_domain() << std::endl << wiped << std::endl; 
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateAnd::propagate(const int changed_idx, 
							     const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  // std::cout << std::endl << "PROPAGATE " << this << std::endl;
  // std::cout << scope[0].get_domain() << " & " 
  // 	    << scope[1].get_domain() << " <-> "
  // 	    << scope[2].get_domain() << std::endl; 

  if(changed_idx == 2) {
    if(LB_CHANGED(evt)) {
      if( IS_FAIL(scope[0].remove(0)) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].remove(0)) ) wiped = FAILURE(1);   
    } else {
      if( scope[1].get_min() ){
	if( IS_FAIL(scope[0].set_domain(0)) ) wiped = FAILURE(0);
      } else if( scope[0].get_min() ) {
	if( IS_FAIL(scope[1].set_domain(0)) ) wiped = FAILURE(1);
      }
    }
  } else { // either z is not yet set, or it is a not(x and y) constraint
    if( scope[2].is_ground() ) {
      if(LB_CHANGED(evt)) {
	// it is an "not(AND)" and one of the variable was set to 1
	if(IS_FAIL(scope[1-changed_idx].set_domain(0))) wiped = FAILURE(1-changed_idx);
      } 
    } else {
      if(UB_CHANGED(evt)) {
	if(IS_FAIL(scope[2].set_domain(0))) wiped = FAILURE(2);
      } else if(scope[1-changed_idx].is_ground()) {
	if(IS_FAIL(scope[2].remove(0))) wiped = FAILURE(2);
      }
    }
  }

 // std::cout << scope[0].get_domain() << " & " 
 // 	    << scope[1].get_domain() << " <-> "
 // 	   << scope[2].get_domain() << std::endl << wiped << std::endl; 
  
  return wiped;
}

std::ostream& Mistral::PredicateAnd::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " <=> (" << scope[0]/*.get_var()*/ << " and " << scope[1]/*.get_var()*/ << ")";
  return os;
}


void Mistral::PredicateOr::initialise() {
  ConstraintImplementation::initialise();
  for(int i=0; i<3; ++i)
    trigger_on(_VALUE_, scope[i]);
}

// Mistral::PropagationOutcome Mistral::PredicateOr::rewrite() {
//   Mistral::PropagationOutcome wiped = propagate();
//   if( scope[2].is_ground() && active.size == 2 ) {
//     relax();
//     if( scope[2].get_min() ) {
//       solver->add(new ConstraintOr(scope));
//     }
//   }
//   return wiped;
// }


Mistral::PropagationOutcome Mistral::PredicateOr::propagate(const int changed_idx, 
							    const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if(changed_idx == 2) {
    if(UB_CHANGED(evt)) {
      if( IS_FAIL(scope[0].set_domain(0)) ) wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_domain(0)) ) wiped = FAILURE(1);
    } else {
      if( scope[1].get_max() == 0 ){
	if( IS_FAIL(scope[0].remove(0)) ) wiped = FAILURE(0);
      } else if( scope[0].get_max() == 0 ) {
	if( IS_FAIL(scope[1].remove(0)) ) wiped = FAILURE(1);
      }
    } 
  } else { // either z is not yet set, or it is a not(x and y) constraint
    if( scope[2].is_ground() ) {
      if(UB_CHANGED(evt)) {
	// it is an "OR" constraint
	if(IS_FAIL(scope[1-changed_idx].remove(0))) wiped = FAILURE(1-changed_idx);
      }
    } else { 
      if(LB_CHANGED(evt)) {
	if( IS_FAIL(scope[2].remove(0)) ) wiped = FAILURE(2);
      } else if(scope[1-changed_idx].is_ground()) {
	if( IS_FAIL(scope[2].set_domain(0)) ) wiped = FAILURE(2);
      }
    }
  }

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateOr::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[2].is_ground() ) {
    if( scope[2].get_max() == 0 ) {
      if( IS_FAIL(scope[0].set_domain(0)) ) 
	wiped = FAILURE(0);
      else if( IS_FAIL(scope[1].set_domain(0)) ) 
	wiped = FAILURE(1);
    } else if( scope[2].get_min() ) {
      if( scope[1].get_max() == 0 ){
	if( IS_FAIL(scope[0].remove(0)) ) wiped = FAILURE(0);
      } else if( scope[0].get_max() == 0 ) {
	if( IS_FAIL(scope[1].remove(0)) ) wiped = FAILURE(1);
      }
    } 
  } else {
    if( scope[0].get_min() || scope[1].get_min() ) {
      if( IS_FAIL(scope[2].remove(0)) ) wiped = FAILURE(2);
    } else if( !scope[0].get_max() && !scope[1].get_max() ) {
      if( IS_FAIL(scope[2].set_domain(0)) ) wiped = FAILURE(2);
    }
  }

  return wiped;
}

std::ostream& Mistral::PredicateOr::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " <=> (" << scope[0]/*.get_var()*/ << " or " << scope[1]/*.get_var()*/ << ")";
  return os;
}


void Mistral::ConstraintAnd::initialise() {
  ConstraintImplementation::initialise();
  
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_DOMAIN_, scope[1]);
}

Mistral::PropagationOutcome Mistral::ConstraintAnd::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( IS_FAIL(scope[0].remove(0)) ) 
    wiped = FAILURE(0);
  else if( IS_FAIL(scope[1].remove(0)) ) 
    wiped = FAILURE(1);
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::ConstraintAnd::propagate(const int changed_idx, 
							      const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( IS_FAIL(scope[0].remove(0)) ) 
    wiped = FAILURE(0);
  else if( IS_FAIL(scope[1].remove(0)) ) 
    wiped = FAILURE(1);
  
  return wiped;
}

std::ostream& Mistral::ConstraintAnd::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/ << " and " << scope[1]/*.get_var()*/ << ")";
  return os;
}


void Mistral::ConstraintNotAnd::initialise() {
  ConstraintImplementation::initialise();
  
  trigger_on(_DOMAIN_, scope[0]);
  trigger_on(_DOMAIN_, scope[1]);
}

Mistral::PropagationOutcome Mistral::ConstraintNotAnd::propagate() {   
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].get_min() ){
    if( IS_FAIL(scope[0].remove(1)) ) wiped = FAILURE(0);
  } else if( scope[0].get_min() ) {
    if( IS_FAIL(scope[1].remove(1)) ) wiped = FAILURE(1);
  }
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::ConstraintNotAnd::propagate(const int changed_idx, 
							      const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if(LB_CHANGED(evt) && IS_FAIL(scope[1-changed_idx].remove(1))) wiped = FAILURE(1-changed_idx);
  
  return wiped;
}

std::ostream& Mistral::ConstraintNotAnd::display(std::ostream& os) const {
  os << "not(" << scope[0]/*.get_var()*/ << " and " << scope[1]/*.get_var()*/ << ")";
  return os;
}

void Mistral::ConstraintOr::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_VALUE_, scope[0]);
  trigger_on(_VALUE_, scope[1]);
}

Mistral::PropagationOutcome Mistral::ConstraintOr::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if( scope[1].get_max() == 0 ){
    if( IS_FAIL(scope[0].remove(0)) ) wiped = FAILURE(0);
  } else if( scope[0].get_max() == 0 ) {
    if( IS_FAIL(scope[1].remove(0)) ) wiped = FAILURE(1);
  }
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::ConstraintOr::propagate(const int changed_idx, 
							     const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  if(UB_CHANGED(evt) && IS_FAIL(scope[1-changed_idx].remove(0))) wiped = FAILURE(1-changed_idx);
  
  return wiped;
}

std::ostream& Mistral::ConstraintOr::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/ << " or " << scope[1]/*.get_var()*/ << ")";
  return os;
}


void Mistral::ConstraintLex::initialise() {
  ConstraintImplementation::initialise();
  for(int i=0; i<4; ++i)
    trigger_on(_RANGE_, scope[i]);
  GlobalConstraint::initialise();
}

void Mistral::ConstraintLex::mark_domain() {
  for(int i=0; i<4; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}



Mistral::PropagationOutcome Mistral::ConstraintLex::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;
  Event evt, aux;

  /*
    rules:
    
    1: IF b1=0 THEN x0 = x1     ACTIVATION: change on b1 or state&1 and change on x0 or x1
    2: IF x0 =/= x1 THEN b1=1   ACTIVATION: change on x0 or x1

    4: IF b0<b1 THEN x0<x1      ACTIVATION: change on b0 or b1 or state&4 and change on x0 or x1
    8: IF x0>=x1 THEN b0=b1     ACTIVATION: change on x0 or x1 or state&8 and change on b0 or b1

    16: IF b0=1 THEN b1=1       ACTIVATION: change on b0 or b1
    32: IF b1=0 THEN b0=0       ACTIVATION: change on b0 or b1
   */


  int state = (!scope[3].get_max() + 
	       4*(scope[2].get_max() < scope[3].get_min()) + 
	       8*(scope[0].get_min()>=scope[1].get_max()));


#ifdef _DEBUG_LEX
  std::cout << std::endl;
#endif


  while(IS_OK(wiped) && !changes.empty()) {
    evt = changes.pop();
    
#ifdef _DEBUG_LEX
    std::cout << " change on " << scope[evt] << " in " << scope[evt].get_domain() 
	      << " state: ( " ;
    if(state&1) std::cout << scope[3] << "=1 ";
    if(state&4) std::cout << scope[2] << "<" << scope[3] << " ";
    if(state&8) std::cout << scope[0] << ">=" << scope[1] << " ";
    std::cout << ")"<< std::endl;  
#endif
    
    if(evt < 2) {
      if(state&1) {

#ifdef _DEBUG_LEX
	std::cout << "Rule1: " << scope[0] << " = " << scope[1] << std::endl;
#endif

	// state&1 and change on x0 or x1 => R1
	FILTER2(aux,  1-evt, set_domain(scope[evt]) );
      }
      if(IS_OK(wiped) && state&4) {

#ifdef _DEBUG_LEX
	std::cout << "Rule3: " << scope[0] << " < " << scope[1] << std::endl;
#endif

	// state&4 and change on x0 or x1 => R3
	FILTER2(aux,  0, set_max(scope[1].get_max()-1) );
	FILTER2(aux,  1, set_min(scope[0].get_min()+1) );
      }

      // change on x0 or x1 => R2
      if(IS_OK(wiped) &&  !(state&1) && !scope[0].intersect(scope[1])) {

#ifdef _DEBUG_LEX
	std::cout << "Rule2: " << scope[3] << " = 1" << std::endl;
#endif

	FILTER2(aux,  3, set_domain(1) );
	//state |= 1;
	if(!scope[2].get_max()) state |= 4;
      }

      // change on x0 or x1 => R4
      if(IS_OK(wiped) &&  scope[0].get_min() >= scope[1].get_max()) {

#ifdef _DEBUG_LEX
	std::cout << "Rule4: " << scope[2] << " = " << scope[3] << std::endl;
#endif

	FILTER2(aux,  3, set_domain(scope[2]) );
	FILTER2(aux,  2, set_domain(scope[3]) );
	state |= 8;
	if(!scope[3].get_max()) state |= 1;
      }

    } else {
      
      if(evt == 3) {
	if(!scope[3].get_max()) {

#ifdef _DEBUG_LEX
	  std::cout << "Rule1: " << scope[0] << " = " << scope[1] << std::endl;
#endif
	  
	  state |= 1;
	  // change on b1 => R1
	  FILTER2(aux,  0, set_domain(scope[1]) );
	  FILTER2(aux,  1, set_domain(scope[0]) );


#ifdef _DEBUG_LEX
	  std::cout << "Rule6: " << scope[2] << " = 0 " << std::endl;
#endif

	  FILTER2(aux,  2, set_domain(0) );

	}
      } else {
	if( scope[2].get_min() ) {
	  
#ifdef _DEBUG_LEX
	  std::cout << "Rule5: " << scope[1] << " = 1 " << std::endl;
#endif

	  FILTER2(aux,  3, set_domain(1) );

	}
      }

      // state&8 and change on b0 or b1 => R4
      if(state&8) {

#ifdef _DEBUG_LEX
	std::cout << "Rule4: " << scope[2] << " = " << scope[3] << std::endl;
#endif

	FILTER2(aux,  5-evt, set_domain(scope[evt].get_min()) );

	if(!scope[3].get_max()) state |= 1;
      }

      // change on b0 or b1 => R3
      if(scope[2].get_max() < scope[3].get_min()) {

#ifdef _DEBUG_LEX
	std::cout << "Rule3: " << scope[0] << " < " << scope[1] << std::endl;
#endif


	FILTER2(aux,  0, set_max(scope[1].get_max()-1) );
	FILTER2(aux,  1, set_min(scope[0].get_min()+1) );
      }
    }    
  }

#ifdef _DEBUG_LEX
  if(!IS_OK(wiped))
    std::cout << "wipe out of x" << wiped << std::endl; 
#endif
  
  return wiped;
}

std::ostream& Mistral::ConstraintLex::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/ << " lex " << scope[1]/*.get_var()*/ << " - " << scope[2]/*.get_var()*/ << "." << scope[3]/*.get_var()*/ << ")";
  return os;
}


// void Mistral::ConstraintLexf::initialise() {
//   Constraint::initialise();
//   for(int i=0; i<3; ++i)
//     trigger_on(_RANGE_, i);
//   set_idempotent(true);
// }

// Mistral::PropagationOutcome Mistral::ConstraintLexf::propagate() {      
//   Mistral::PropagationOutcome wiped = CONSISTENT;
//   Event evt;

//   while(!changes.empty()) {
//     evt = changes.pop();

//     if(evt==2) {
//       if(scope[2].get_min()) {
// 	// => x0 < x1
// 	FILTER( 0, set_max(scope[1].get_max()-1) );
// 	FILTER( 1, set_min(scope[0].get_min()+1) );
//       } else {
// 	// => x0 = x1
// 	FILTER( 0, set_domain(scope[1]) );
// 	FILTER( 1, set_domain(scope[0]) );
//       }
//     } else {
//       if(scope[2].get_min()) {
// 	if(evt) FILTER( 0, set_max(scope[1].get_max()-1) );
// 	else FILTER( 1, set_min(scope[0].get_min()+1) );
//       } else if(!scope[2].get_max()) {
// 	FILTER( 1-evt, set_domain(scope[evt]) );
//       } else {
// 	// in anycase x0 <= x1
// 	FILTER( 0, set_max(scope[1].get_max()) );
// 	FILTER( 1, set_min(scope[0].get_min()) );
//       }
//     }
//   }  
  
//   return wiped;
// }

// std::ostream& Mistral::ConstraintLexf::display(std::ostream& os) const {
//   os << "(" << scope[0] << " lex " << scope[1] << ")";
//   return os;
// }


void Mistral::PredicateAdd::initialise() {

  ConstraintImplementation::initialise();

  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  trigger_on(_RANGE_, scope[2]);

  //std::cout << "scope: " << _scope << std::endl;
}


void Mistral::PredicateAdd::mark_domain() {
  for(int i=0; i<3; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}

Mistral::PropagationOutcome Mistral::PredicateAdd::rewrite() {
  // TODO: recode that
  
  Mistral::PropagationOutcome wiped = propagate();

  VarArray tmp;
  if(active == 3) {
    ConstraintImplementation *con;
    int i=0;
    for(; i<2; ++i)
      if(scope[i].is_ground()) {
	relax();
	tmp.add(scope[1-i]);
	tmp.add(scope[2]);
	if(scope[i].get_min() == 0) {
	  con = new ConstraintEqual(tmp);
	} else {
	  con = new PredicateOffset(tmp, scope[i].get_min());
	}
	get_solver()->add(Constraint(con, con->type));
      }
  }
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateAdd::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

  int ub_not_revised = 7;
  int lb_not_revised = 7;
  Event cevt;


#ifdef _DEBUG_ADD
  if(_DEBUG_ADD) {
    std::cout << "init prop "
	      << scope[0].get_domain() << " + " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << std::endl;
  }
#endif

  while(ub_not_revised || lb_not_revised) {
    
    if(lb_not_revised&1) {
      // revise scope[0]'lb
      cevt = scope[0].set_min(scope[2].get_min() - scope[1].get_max());
      if(IS_FAIL(cevt)) {
	wiped = FAILURE(0);
	break;
      } else if(cevt != NO_EVENT) {
	// scope[0]'s lb changed and may trigger scope[2]'s lb or scope[1]'s ub
	lb_not_revised |= 4;
	ub_not_revised |= 2;
      }
      lb_not_revised ^= 1;
    }

    if(ub_not_revised&1) {
      // revise scope[0]'ub
      cevt = scope[0].set_max(scope[2].get_max() - scope[1].get_min());
      if(IS_FAIL(cevt)) {
	wiped = FAILURE(0);
	break;
      } else if(cevt != NO_EVENT) {
	// scope[0]'s ub changed and may trigger scope[2]'s ub or scope[1]'s lb
	ub_not_revised |= 4;
	lb_not_revised |= 2;
      }
      ub_not_revised ^= 1;
    }


    if(lb_not_revised&2) {
      // revise scope[1]'lb
      cevt = scope[1].set_min(scope[2].get_min() - scope[0].get_max());
      if(IS_FAIL(cevt)) {
	wiped = FAILURE(1);
	break;
      } else if(cevt != NO_EVENT) {
	// scope[1]'s lb changed and may trigger scope[2]'s lb or scope[0]'s ub
	lb_not_revised |= 4;
	ub_not_revised |= 1;
      }
      lb_not_revised ^= 2;
    }

    if(ub_not_revised&2) {
      // revise scope[1]'ub
      cevt = scope[1].set_max(scope[2].get_max() - scope[0].get_min());
      if(IS_FAIL(cevt)) {
	wiped = FAILURE(1);
	break;
      } else if(cevt != NO_EVENT) {
	// scope[1]'s ub changed and may trigger scope[2]'s ub or scope[0]'s lb
	ub_not_revised |= 4;
	lb_not_revised |= 1;
      }
      ub_not_revised ^= 2;
    }


    if(lb_not_revised&4) {
      // revise scope[2]'lb
      cevt = scope[2].set_min(scope[0].get_min() + scope[1].get_min());
      if(IS_FAIL(cevt)) {
	wiped = FAILURE(2);
	break;
      } else if(cevt != NO_EVENT) {
	// scope[2]'s lb changed and may trigger scope[0]'s lb or scope[1]'s lb
	lb_not_revised |= 2;
	lb_not_revised |= 1;
      }
      lb_not_revised ^= 4;
    }

    if(ub_not_revised&4) {
      // revise scope[2]'ub
      cevt = scope[2].set_max(scope[0].get_max() + scope[1].get_max());
      if(IS_FAIL(cevt)) {
	wiped = FAILURE(2);
	break;
      } else if(cevt != NO_EVENT) {
	// scope[2]'s ub changed and may trigger scope[0]'s ub or scope[1]'s ub
	ub_not_revised |= 2;
	ub_not_revised |= 1;
      }
      ub_not_revised ^= 4;
    }
  }

#ifdef _DEBUG_ADD
  if(_DEBUG_ADD) {
    std::cout << scope[0].get_domain() << " + " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif

  return wiped;
}


Mistral::PropagationOutcome Mistral::PredicateAdd::propagate(const int changed_idx, 
							     const Event evt) {      
  Mistral::PropagationOutcome wiped = CONSISTENT;
  int cidx = changed_idx;
  int cevt = evt;

#ifdef _DEBUG_ADD
  if(_DEBUG_ADD) {
    std::cout << scope[0] << " + " 
	      << scope[1] << " = " 
	      << scope[2] << std::endl; 
    std::cout << scope[0].get_domain() << " + " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << " " << event2str(evt) 
	      << " on " << scope[changed_idx].get_domain() << std::endl 
	      << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl;
  }
#endif

  //_x_ + y = z
  
  //do {
  if(cidx == 0) {
    if(LB_CHANGED(evt)) {
      // max(y) = max(z) - min(x) 
      // update y's ub
      cevt = scope[1].set_max(scope[2].get_max() - scope[0].get_min());
      if(IS_FAIL(cevt)) wiped = FAILURE(1);
	
	if(IS_OK(wiped)) {
	  // min(z) = min(x) + min(y);
	  // update z's lb
	  cevt = scope[2].set_min(scope[0].get_min() + scope[1].get_min());
	  if(IS_FAIL(cevt)) wiped = FAILURE(2);
	}
      }
      if(UB_CHANGED(evt)) {
	// min(y) = min(z) - max(x) 
	// update y's lb
	cevt = scope[1].set_min(scope[2].get_min() - scope[0].get_max());
	if(IS_FAIL(cevt)) wiped = FAILURE(1);
	
	if(IS_OK(wiped)) {
	  // max(z) = max(x) + max(y)
	  // update z's ub
	  cevt = scope[2].set_max(scope[0].get_max() + scope[1].get_max());
	  if(IS_FAIL(cevt)) wiped = FAILURE(2);
	}
      }
    } else if(cidx == 1) {
      if(LB_CHANGED(evt)) {
	// max(x) = max(z) - min(y) 
	// update x's ub
	cevt = scope[0].set_max(scope[2].get_max() - scope[1].get_min());
	if(IS_FAIL(cevt)) wiped = FAILURE(0);
	
	if(IS_OK(wiped)) {
	  // min(z) = min(x) + min(y);
	  // update z's lb
	  cevt = scope[2].set_min(scope[0].get_min() + scope[1].get_min());
	  if(IS_FAIL(cevt)) wiped = FAILURE(2);
	}
      }
      if(UB_CHANGED(evt)) {
	// min(x) = min(z) - max(y) 
	// update x's lb
	cevt = scope[0].set_min(scope[2].get_min() - scope[1].get_max());
	if(IS_FAIL(cevt)) wiped = FAILURE(0);

	if(IS_OK(wiped)) {	
	  // max(z) = max(x) + max(y)
	  // update z's ub
	  cevt = scope[2].set_max(scope[0].get_max() + scope[1].get_max());
	  if(IS_FAIL(cevt)) wiped = FAILURE(2);
	}
      }
    } else {
      if(UB_CHANGED(evt)) {
	// max(x) = max(z) - min(y) 
	// update x's ub
	cevt = scope[0].set_max(scope[2].get_max() - scope[1].get_min());
	if(IS_FAIL(cevt)) wiped = FAILURE(0);

	if(IS_OK(wiped)) {	
	  // max(y) = max(z) - min(x);
	  // update y's ub
	  cevt = scope[1].set_max(scope[2].get_max() - scope[0].get_min());
	  if(IS_FAIL(cevt)) wiped = FAILURE(1);
	}
      }
      if(LB_CHANGED(evt)) {
	// min(x) = min(z) - max(y) 
	// update x's lb
	cevt = scope[0].set_min(scope[2].get_min() - scope[1].get_max());
	if(IS_FAIL(cevt)) wiped = FAILURE(0);

	if(IS_OK(wiped)) {	
	  // min(y) = min(z) - max(x) 
	  // update y's lb
	  cevt = scope[1].set_min(scope[2].get_min() - scope[0].get_max());
	  if(IS_FAIL(cevt)) wiped = FAILURE(1);
	}
      }
      //}
    
    if(IS_OK(wiped)) {
      update(cidx, cevt);
    }
  }

#ifdef _DEBUG_ADD
  if(_DEBUG_ADD) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl
	      << scope[0].get_domain() << " + " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif
  
  return wiped;
}

std::ostream& Mistral::PredicateAdd::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " + " << scope[1]/*.get_var()*/ << ")";
  return os;
}





// int __modulo_fct__(const int x, const int m) {
//   int mod = x%m;
//   if(mod && (mod<0) != (m<0))  mod += m;
//   return mod;
// }


// /*
//   [HERE WE ASSUME 0<k and 0<=a<=b]

// ================================
// |  min([c, d]) such that       |
// |  ([a, b] % k) = [c, d]       |
// |                              |
// |  if k>b: a                   |
// |                              |
// |  if a<=k<=b: 0               |
// |                              |
// |  so we can assume that k < a |
// |                              |
// |  if (a%k + b - a) >= k: 0    |
// |  else: a%k                   |
// ================================

// [IF k>0 THEN:]

// ================================
// |  min([c, d]) such that       |
// |  ([a, b] % k) = [c, d]       |
// |                              |
// |  if k>b: a                   |
// |                              |
// |  if a<=k<=b: 0               |
// |                              |
// |  so we can assume that k < a |
// |                              |
// |  if (a%k + b - a) >= k: 0    |
// |  else: a%k                   |
// ================================


// */
// int min_modulo(const int a, const int b, const int k) {
//   int value = a;
//   if(k<=b) {
//     if(k>=a) value = 0;
//     else {
//       int mod = __modulo_fct__(a,k);
//       if((mod + b - a) >= k) value = 0;
//       else value = mod;
//     }
//   }
//   return value;
// }
// /*
// ================================
// |  max([c, d]) such that       |
// |  ([a, b] % k) = [c, d]       |
// |                              |
// |  if k>b: b                   |
// |                              |
// |  if a<=k<=b: k-1             |
// |                              |
// |  so we can assume that k < a |
// |                              |
// |  if (b%k - b + a) < 0: k-1   |
// |  else: b%k                   |
// ================================
// */
// int max_modulo(const int a, const int b, const int k) {
//   int value = b;
//   if(k<=b) {
//     if(k>=a) value = k-1;
//     else {
//       int mod = __modulo_fct__(b,k);
//       if((mod - b + a) < 0) value = k-1;
//       else value = mod;
//     }
//   }
//   return value;
// }
// /*
// ================================
// |  min([a, b]) such that       |
// |  ([a, b] % k) = [c, d]       |
// |                              |
// |  if(a%k < c)                 |
// |    a + (c - a%k)             |
// |  if(a%k > d)                 |
// |    a + (d - a%k) + k         |
// |  otherwise: a                |
// ================================
// */
// int min_antimodulo(const int a, const int c, const int d, const int k) {
//   int value = a, mod = a%k;
//   if(mod < c) 
//     value = a + c - mod;
//   else if(mod > d) 
//     value = a + d - mod + k;
//   return value;
// }
// /*
// ================================
// |  max([a, b]) such that       |
// |  ([a, b] % k) = [c, d]       |
// |                              |
// |  if(b%k < c)                 |
// |    b - (b%k - c) - k         |
// |  if(b%k > d)                 |
// |    b - (b%k - d)             |
// |  otherwise: b                |
// ================================
// */
// int max_antimodulo(const int b, const int c, const int d, const int k) {
//   int value = b, mod = b%k;
//   if(mod < c) 
//     value = b - mod + c - k;
//   else if(mod > d) 
//     value = b - mod + d;
//   return value;
// }


void Mistral::PredicateModConstant::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
}

void Mistral::PredicateModConstant::mark_domain() {
  for(int i=0; i<2; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}


Mistral::PropagationOutcome Mistral::PredicateModConstant::filter() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;



#ifdef _DEBUG_MOD
  if(_DEBUG_MOD) {
    std::cout << scope[0] << " % " 
	      << modulo << " = " 
	      << scope[1] << std::endl; 
    std::cout << scope[0].get_domain() << " % " 
	      << modulo << " = " 
	      << scope[1].get_domain() << std::endl 
	      << on[0] << std::endl
	      << on[1] << std::endl;
  }
#endif

  Interval X(scope[0].get_min(), scope[0].get_max());
  //Interval Y(scope[1].get_min(), scope[1].get_max());

  Interval Y = X.operator_modulo(modulo);
  
  if(IS_FAIL(scope[1].set_max(Y.max))) wiped = FAILURE(1);
  else if(IS_FAIL(scope[1].set_min(Y.min))) wiped = FAILURE(1);
  
  if(IS_OK(wiped)) {
    // now we compute forbidden intervals for X
    
    // because of Y's lb:
    int min_y = scope[1].get_min();
    int min_x = scope[0].get_min();
    
    
    int lb, ub, incr;
    
    if(modulo>0) {
      lb = min_x - Mistral::__modulo_fct__(min_x,modulo);
      ub = lb + min_y - 1;
      incr = modulo;
    } else {
      lb = min_x + modulo + 1 - Mistral::__modulo_fct__(min_x,modulo);
      ub = lb - modulo + min_y - 2;
      incr = -modulo;
    }

    
    
#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << "x: " << scope[0].get_domain() 
		<< "\nmin(x)%" << modulo << " = "
		<< Mistral::__modulo_fct__(min_x,modulo)
		<< "\nlb = " << lb
		<< "\nub = " << ub << "\n";
    }
#endif
    
    if(lb<=ub) {
      while(IS_OK(wiped) && lb <= scope[0].get_max()) {

	//std::cout << "remove interval [" << lb << "," << ub << "]" << std::endl;
	
	if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	lb += incr;
	ub += incr;
      }
    }
    
    
    if(IS_OK(wiped)) {
      // because of Y's lb:
      int max_y = scope[1].get_max();
      int max_x = scope[0].get_max();
      
      if(modulo > 0) {
	ub = max_x - Mistral::__modulo_fct__(max_x,modulo) + modulo - 1;
	lb = ub + max_y - modulo + 2;
	incr = modulo;
      } else {
	ub = max_x - Mistral::__modulo_fct__(max_x,modulo);
	lb = ub + max_y + 1;
	incr = -modulo;
      }
      
      
      

      // std::cout << "\nx: " << scope[0].get_domain() 
      // 		<< "\nmax(x)%" << modulo << " = "
      // 		<< Mistral::__modulo_fct__(max_x,modulo)
      // 		<< "\nlb = " << lb
      // 		<< "\nub = " << ub << "\n";
      

      
      if(lb<=ub) {
	while(IS_OK(wiped) && ub >= scope[0].get_min()) {

	  //std::cout << "remove interval [" << lb << "," << ub << "]" << std::endl;

	  if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	  lb -= incr;
	  ub -= incr;
	}
      }
    }

  }




#ifdef _DEBUG_MOD
  if(_DEBUG_MOD) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << scope[0].get_domain() << " % " 
	      << modulo << " = " 
	      << scope[1].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif
  

  
  // if(IS_FAIL(scope[1].set_max(max_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[1].set_min(min_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[0].set_min(min_antimodulo(scope[0].get_min(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);
  // if(IS_FAIL(scope[0].set_max(max_antimodulo(scope[0].get_max(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateModConstant::propagate() {      
  return filter();
}

Mistral::PropagationOutcome Mistral::PredicateModConstant::propagate(const int changed_idx, const Event evt) {
  return filter();
  // Mistral::PropagationOutcome wiped = CONSISTENT;


  // if(IS_FAIL(scope[1].set_max(max_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[1].set_min(min_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[0].set_min(min_antimodulo(scope[0].get_min(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);
  // if(IS_FAIL(scope[0].set_max(max_antimodulo(scope[0].get_max(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);

  // return wiped;
}

std::ostream& Mistral::PredicateModConstant::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " % " << modulo << ")";
  return os;
}



void Mistral::PredicateCModConstant::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
}

void Mistral::PredicateCModConstant::mark_domain() {
  for(int i=0; i<2; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}



Mistral::PropagationOutcome Mistral::PredicateCModConstant::filter() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

#ifdef _DEBUG_CMOD
  if(_DEBUG_CMOD) {
    std::cout << scope[0] << " % " 
	      << modulo << " = " 
	      << scope[1] << std::endl; 
    std::cout << scope[0].get_domain() << " % " 
	      << modulo << " = " 
	      << scope[1].get_domain() << std::endl 
	      << on[0] << std::endl
	      << on[1] << std::endl;
  }
#endif

  Interval X(scope[0].get_min(), scope[0].get_max());
  //Interval Y(scope[1].get_min(), scope[1].get_max());

  Interval Y = X%modulo;


#ifdef _DEBUG_CMOD
  if(_DEBUG_CMOD) {
    std::cout << " => prune " << scope[1] << ": " << scope[1].get_domain() << " <- " << Y << std::endl;
  }
#endif

  
  if(IS_FAIL(scope[1].set_max(Y.max))) wiped = FAILURE(1);
  else if(IS_FAIL(scope[1].set_min(Y.min))) wiped = FAILURE(1);
  
  if(IS_OK(wiped)) {
    // now we compute forbidden intervals for X
    // because of Y's lb:
    int min_y = scope[1].get_min();
    int max_y = scope[1].get_max();

    int min_x = scope[0].get_min();
    int max_x = scope[0].get_max();
    
    int lb, ub, incr = abs(modulo);


#ifdef _DEBUG_CMOD
    if(_DEBUG_CMOD) {
      std::cout << " => prune " << scope[0] << std::endl;
    }
#endif
    
    if(IS_OK(wiped)) {

      if(min_y > 0) { // the image of the modulo is positive, so the domain must be positive

#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	  std::cout << "   => " << scope[1]  << " in " << scope[1].get_domain() 
		    << " is positive, hence "<< scope[0] << " must be positive " << std::endl;
	}
#endif	  
	if(IS_FAIL(scope[0].set_min(1))) wiped = FAILURE(0);
	

	// we know x is positive
	min_x = scope[0].get_min();
	lb = min_x - min_x % modulo;
	ub = lb + min_y - 1;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && lb <= max_x) {
#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	    std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
	}
#endif	
	    if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    lb += incr;
	    ub += incr;
	  }
	  min_x = scope[0].get_min();
	  max_x = scope[0].get_max();
	}
      } else if(max_y < 0) { // the image of the modulo is negative, so the domain must be negative
	if(IS_FAIL(scope[0].set_max(-1))) wiped = FAILURE(0);
	

#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	  std::cout << "   => " << scope[1]  << " in " << scope[1].get_domain() 
		    << " is negative, hence "<< scope[0] << " must be negative " << std::endl;
	}
#endif

	// we know x is negative
	max_x = scope[0].get_max();
	// pruning because of Y's ub on the negative half of X
	ub = max_x - max_x % modulo;
	lb = ub + max_y + 1;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && ub >= min_x) {
#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	    std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
	}
#endif	
	    if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    lb -= incr;
	    ub -= incr;
	  }
	  min_x = scope[0].get_min();
	  max_x = scope[0].get_max();
	}
      } //else { // min(y) < 0 < max(y)
	
      if(max_y < incr-1) {


#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	  std::cout << "   => " << scope[1]  << " in " << scope[1].get_domain() 
		    << " < " << incr << ", hence we prune " << scope[0] << std::endl;
	}
#endif

	// pruning because of Y's ub on the positive half of X
	if(min_x > 0) {
	  ub = min_x - min_x % modulo + incr - 1;
	} else {
	  ub = incr-1;
	}
	lb = ub - incr + max_y + 2;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && lb <= max_x) {
#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	    std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
	}
#endif	
	    if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    lb += incr;
	    ub += incr;
	  }
	  min_x = scope[0].get_min();
	  max_x = scope[0].get_max();
	}
      }
      
      if(min_y > 1-incr) {

#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	  std::cout << "   => " << scope[1]  << " in " << scope[1].get_domain() 
		    << " > " << -incr << ", hence we prune " << scope[0] << std::endl;
	}
#endif

	// pruning because of Y's lb on the negative half of X
	if(max_x < 0) {
	  lb = max_x - max_x % modulo - incr + 1;
	} else {
	  lb = 1-incr;
	}
	ub = lb + incr + min_y - 2;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && ub >= min_x) {
#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	    std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
	}
#endif	
	    if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    lb -= incr;
	    ub -= incr;
	  }
	  min_x = scope[0].get_min();
	  max_x = scope[0].get_max();
	}
      } 
    }    
  }
  
  

#ifdef _DEBUG_CMOD
  if(_DEBUG_CMOD) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << scope[0].get_domain() << " % " 
	      << modulo << " = " 
	      << scope[1].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif
  

  
  // if(IS_FAIL(scope[1].set_max(max_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[1].set_min(min_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[0].set_min(min_antimodulo(scope[0].get_min(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);
  // if(IS_FAIL(scope[0].set_max(max_antimodulo(scope[0].get_max(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateCModConstant::propagate() {      
  return filter();
}

Mistral::PropagationOutcome Mistral::PredicateCModConstant::propagate(const int changed_idx, const Event evt) {
  return filter();
  // Mistral::PropagationOutcome wiped = CONSISTENT;


  // if(IS_FAIL(scope[1].set_max(max_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[1].set_min(min_modulo(scope[0].get_min(), scope[0].get_max(), modulo)))) wiped = FAILURE(1);
  // if(IS_FAIL(scope[0].set_min(min_antimodulo(scope[0].get_min(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);
  // if(IS_FAIL(scope[0].set_max(max_antimodulo(scope[0].get_max(), 
  // 					     scope[1].get_min(), 
  // 					     scope[1].get_max(), modulo)))) wiped = FAILURE(0);

  // return wiped;
}

std::ostream& Mistral::PredicateCModConstant::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " % " << modulo << ")";
  return os;
}




/*


  [HERE WE ASSUME 0<=a<=b AND 0<=c<=d]

  min([a,b] % [c,d]) =

  if c>b: a

  if [a,b] inter [c,d]: 0


  so we can assume that d < a

  if (b-a+1) >= c: 0 
     
min(k \in [c,d])
  if (a%k + b - a) >= k: 0
  else: a%k

 */


void Mistral::PredicateMod::initialise() {
  ConstraintImplementation::initialise();

  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  trigger_on(_RANGE_, scope[2]);
}

void Mistral::PredicateMod::mark_domain() {
  for(int i=0; i<3; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}



Mistral::PropagationOutcome Mistral::PredicateMod::rewrite() {
   Mistral::PropagationOutcome wiped = propagate();



  // VarArray tmp;
  // if(active == 3) {
  //   ConstraintImplementation *con;
  //   int i=0;
  //   for(; i<2; ++i)
  //     if(scope[i].is_ground()) {
  // 	relax();
  // 	tmp.add(scope[1-i]);
  // 	tmp.add(scope[2]);
  // 	if(scope[i].get_min() == 0) {
  // 	  con = new ConstraintEqual(tmp);
  // 	} else {
  // 	  con = new PredicateOffset(tmp, scope[i].get_min());
  // 	}
  // 	get_solver()->add(Constraint(con, con->type));
  //     }
  // }
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateMod::propagate() { 
  if(IS_FAIL(scope[1].remove(0))) return FAILURE(1);
  return filter();
}

Mistral::PropagationOutcome Mistral::PredicateMod::filter() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;


  //std::cout << "\n ^^^" << std::endl;

#ifdef _DEBUG_MOD
  if(_DEBUG_MOD) {
    std::cout << scope[0] << " % " 
	      << scope[1] << " = " 
	      << scope[2] << std::endl; 
    std::cout << scope[0].get_domain() << " % " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << std::endl 
	      << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl;
  }
#endif


  int vnext = scope[1].get_min(), modulo = vnext-1;
  int minmin_x =  INFTY;
  int maxmax_x = -INFTY;

  int minmin_y =  INFTY;
  int maxmax_y = -INFTY;


  IntervalList lb;
  IntervalList ub;

  IntervalList forbidden_intervals;

  IntervalList last_intervals;
  IntervalList cur_intervals;

  forbidden_intervals.push(-INFTY, INFTY);


  while(vnext > modulo) {
    modulo = vnext;


#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " -> modulo = " << modulo << std::endl;
  }
#endif

    Interval X(scope[0].get_min(), scope[0].get_max());
    //Interval Y(scope[1].get_min(), scope[1].get_max());
    
    Interval cur_Y(scope[2].get_min(), scope[2].get_max());

    Interval Y = X.target_modulo(modulo, cur_Y);

    //std::cout << X << "%" << modulo << " ^ " << cur_Y << " = " << Y << std::endl;

    
    if(scope[2].get_min() > Y.max || scope[2].get_max() < Y.min) {


#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " [inconsistent] " << std::endl;
    }
#endif

      if(IS_FAIL(scope[1].remove(modulo))) wiped = FAILURE(1);
    } else {

#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " [ok] " << std::endl;
    }
#endif


      if(minmin_y > Y.min) minmin_y = Y.min;
      if(maxmax_y < Y.max) maxmax_y = Y.max;
      //    }
  
    if(IS_OK(wiped)) {
      //minmin_x = scope[0].get_min();
      //maxmax_x = scope[0].get_max();

      int min_y = scope[2].get_min();
      int min_x = scope[0].get_min();
      int max_y = scope[2].get_max();
      int max_x = scope[0].get_max();


      // the last forbidden interval on the left of x, due to Y's min
      Interval left_lb;
      // the first forbidden interval on the right of x, due to Y's min
      Interval right_lb;

      // the last forbidden interval on the left of x, due to Y's max
      Interval left_ub;
      // the first forbidden interval on the right of x, due to Y's max
      Interval right_ub;
      
      int incr;

        
      if(modulo>0) {
	left_lb.min = 
	  /*start from min(x)*/ min_x 
	  /*move to the next 0 on the left*/ - Mistral::__modulo_fct__(min_x,modulo);
	left_lb.max = left_lb.min + min_y - 1;

	right_lb.min = 
	  /*start from max(x)*/ max_x 
	  /*move to the next 0 on the left*/ - Mistral::__modulo_fct__(max_x,modulo);
	right_lb.max = right_lb.min + min_y - 1;


	left_ub.max = 
	  /*start from min(x)*/ min_x 
	  /*move to the next mod-1 on the right*/ - Mistral::__modulo_fct__(min_x,modulo) + modulo - 1;
	left_ub.min = left_ub.max + max_y - modulo + 2;

	right_ub.max = 
	  /*start from max(x)*/ max_x 
	  /*move to the next mod-1 on the right*/ - Mistral::__modulo_fct__(max_x,modulo) + modulo - 1;
	right_ub.min = right_ub.max + max_y - modulo + 2;

// #ifdef _DEBUG_MOD
//     if(_DEBUG_MOD) {
//       std::cout << " compute lb pruning: " << std::endl;
//     }
// #endif

	lb.clear();
	if(!left_lb.empty()) {
	  Interval I(left_lb.min, left_lb.max);
	  while(I.min <= scope[0].get_max()) {
	    lb.push(I);
	    I += modulo;
	  }
	}

// #ifdef _DEBUG_MOD
//     if(_DEBUG_MOD) {
//       std::cout << " compute ub pruning: " << std::endl;
//     }
// #endif

	ub.clear();
	if(!left_ub.empty()) {
	  Interval I(left_ub.min, left_ub.max);
	  while(I.min <= scope[0].get_max()) {
	    ub.push(I);
	    I += modulo;
	  }
	}

// #ifdef _DEBUG_MOD
//     if(_DEBUG_MOD) {
//       std::cout << " manage interval lists: " << std::endl;
//     }
// #endif

#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " lb pruning: " << lb << std::endl;
      std::cout << " ub pruning: " << ub << std::endl;
    }
#endif


	last_intervals = forbidden_intervals;
	cur_intervals.clear();
	lb.union_with(ub, cur_intervals);
	forbidden_intervals.clear();
	last_intervals.intersect_with(cur_intervals,forbidden_intervals);

#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " all pruning for this round: " << cur_intervals << std::endl;
      std::cout << " all pruning: " << forbidden_intervals << std::endl;
    }
#endif


	incr = modulo;
      } else {
	
	left_lb.min =  
	  /*start from min(x)*/ min_x 
	  /*move to the next 1+mod on the left*/ - Mistral::__modulo_fct__(min_x,modulo) + modulo + 1;
	left_lb.max = left_lb.min - modulo + min_y - 2;

	right_lb.min =  
	  /*start from max(x)*/ max_x 
	  /*move to the next 1+mod on the left*/ - Mistral::__modulo_fct__(max_x,modulo) + modulo + 1;
	right_lb.max = right_lb.min - modulo + min_y - 2;


	left_ub.max =  
	  /*start from min(x)*/ min_x 
	  /*move to the next 0 on the right*/ - Mistral::__modulo_fct__(min_x,modulo);
	left_ub.min = left_ub.max + max_y + 1;

	right_ub.max =  
	  /*start from max(x)*/ max_x 
	  /*move to the next 0 on the right*/ - Mistral::__modulo_fct__(max_x,modulo);
	right_ub.min = right_ub.max + max_y + 1;

	incr = -modulo;

// #ifdef _DEBUG_MOD
//     if(_DEBUG_MOD) {
//       std::cout << " compute -lb pruning: " << std::endl;
//     }
// #endif

	lb.clear();
	if(!left_lb.empty()) {
	  Interval I(left_lb.min, left_lb.max);
	  while(I.min <= scope[0].get_max()) {
	    lb.push(I);
	    I -= modulo;
	  }
	}

// #ifdef _DEBUG_MOD
//     if(_DEBUG_MOD) {
//       std::cout << " compute -ub pruning: " << std::endl;
//     }
// #endif

	ub.clear();
	if(!left_ub.empty()) {
	  Interval I(left_ub.min, left_ub.max);
	  while(I.min <= scope[0].get_max()) {
	    ub.push(I);
	    I -= modulo;
	  }
	}

// #ifdef _DEBUG_MOD
//     if(_DEBUG_MOD) {
//       std::cout << " manage interval lists: " << std::endl;
//     }
// #endif

#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " lb pruning: " << lb << std::endl;
      std::cout << " ub pruning: " << ub << std::endl;
    }
#endif

	last_intervals = forbidden_intervals;
	cur_intervals.clear();
	lb.union_with(ub, cur_intervals);
	forbidden_intervals.clear();
	last_intervals.intersect_with(cur_intervals,forbidden_intervals);

#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " all pruning for this round: " << cur_intervals << std::endl;
      std::cout << " all pruning: " << forbidden_intervals << std::endl;
    }
#endif

      }

      bool shrink = false;
      do {
	shrink = false;
   
#ifdef _DEBUG_MOD
      if(_DEBUG_MOD) {
	std::cout << " -> " << left_lb << "/" << left_ub << " min(x) = " << min_x << std::endl;
      }
      //exit(1);
#endif

	if(!left_lb.empty() && left_lb.min <= min_x && left_lb.max >= min_x) {
	  min_x = left_lb.max+1;
	  shrink = true;
	}
	if(!left_ub.empty() && left_ub.min <= min_x && left_ub.max >= min_x) {
	  min_x = left_ub.max+1;
	  shrink = true;
	}
	
	left_lb += incr;
	left_ub += incr;
      } while(shrink && min_x <= max_x);

      shrink = false;
      do {
#ifdef _DEBUG_MOD
      if(_DEBUG_MOD) {
	std::cout << " -> " << right_lb << "/" << right_ub << " max(x) = " << max_x << std::endl;
      }
      //exit(1);
#endif
      shrink = false;


	if(!right_lb.empty() && right_lb.max >= max_x && right_lb.min <= max_x) {
	  max_x = right_lb.min-1;
	  shrink = true;
	}
	if(!right_ub.empty() && right_ub.max >= max_x && right_ub.min <= max_x) {
	  max_x = right_ub.min-1;
	  shrink = true;
	}
	
	right_lb -= incr;
	right_ub -= incr;
      } while(shrink && min_x <= max_x);


      if(min_x < minmin_x) minmin_x = min_x;
      if(max_x > maxmax_x) maxmax_x = max_x;
    }
    }

#ifdef _DEBUG_MOD
    if(_DEBUG_MOD) {
      std::cout << " => X in [" << minmin_x << "," << maxmax_x << "], Y in [" << minmin_y << "," << maxmax_y << "]\n" << std::endl;
    }
#endif
    
    vnext = scope[1].next(modulo);

  }



  
  // if(IS_OK(wiped) && minmin_x <  INFTY && IS_FAIL(scope[0].set_min(minmin_x))) wiped = FAILURE(0);
  if(IS_OK(wiped) && minmin_y <  INFTY && IS_FAIL(scope[2].set_min(minmin_y))) wiped = FAILURE(2);
  // if(IS_OK(wiped) && maxmax_x > -INFTY && IS_FAIL(scope[0].set_max(maxmax_x))) wiped = FAILURE(0);
  if(IS_OK(wiped) && maxmax_y > -INFTY && IS_FAIL(scope[2].set_max(maxmax_y))) wiped = FAILURE(2);
    
#ifdef _DEBUG_MOD
  if(_DEBUG_MOD) {
    std::cout << " +++> remove " << forbidden_intervals << " from " << scope[0].get_domain() << " / set domain to [" << minmin_x << "," << maxmax_x << "]\n";
  }
#endif

  for(unsigned int i=0; IS_OK(wiped) && i<forbidden_intervals.size; ++i) {
    if(IS_FAIL(scope[0].remove_interval(forbidden_intervals[i].min, forbidden_intervals[i].max))) wiped = FAILURE(0);
  }





  //}
    // if(IS_FAIL(scope[0].set_domain(minmin_x, maxmax_x))) wiped = FAILURE(0);
    // else if(IS_FAIL(scope[2].set_domain(minmin_y, maxmax_y))) wiped = FAILURE(2);
  

#ifdef _DEBUG_MOD
  if(_DEBUG_MOD) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl
	      << scope[0].get_domain() << " % " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif
  

  //std::cout << " vvv" << std::endl;

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateMod::propagate(const int changed_idx, 
							     const Event evt) {    

 return filter();
  
//   Mistral::PropagationOutcome wiped = CONSISTENT;

// #ifdef _DEBUG_MOD
//   if(_DEBUG_MOD) {
//     std::cout << scope[0] << " % " 
// 	      << scope[1] << " = " 
// 	      << scope[2] << std::endl; 
//     std::cout << scope[0].get_domain() << " % " 
// 	      << scope[1].get_domain() << " = " 
// 	      << scope[2].get_domain() << " " << event2str(evt) 
// 	      << " on " << scope[changed_idx].get_domain() << std::endl 
// 	      << on[0] << std::endl
// 	      << on[1] << std::endl
// 	      << on[2] << std::endl;
//   }
// #endif



//   //x % y = z

//   // the initial bounds of z 
//   int min_z = scope[2].get_min();
//   int max_z = scope[2].get_max();
//   // the current bounds of z
//   int lb_z = +INFTY;
//   int ub_z = -INFTY;

  
//   // the initial bounds of x
//   int min_x = scope[0].get_min();
//   int max_x = scope[0].get_max();
//   // the current bounds of x
//   int lb_x = +INFTY;
//   int ub_x = -INFTY;


//   int modmin, modmax, antimodmin, antimodmax;
//   int modnext = scope[1].get_min();
//   int modulo = modnext-1;
//   for(;
//       IS_OK(wiped) && modulo<modnext; 
//       modnext = scope[1].next(modulo)) {

//     modulo = modnext;

//     //compute the min and max possible values for x%modulo, and update the current lb/ub of z accordingly
//     modmin = min_modulo(min_x, max_x, modulo);
//     if(modmin < lb_z) {
//       lb_z = modmin;
//     } 
//     modmax = max_modulo(min_x, max_x, modulo);
//     if(modmax > ub_z) {
//       ub_z = modmax;
//     } 

//     //compute the min and max possible values for x given that x%modulo = z, and update the current lb/ub of x accordingly
//     antimodmin = min_antimodulo(min_x, min_z, max_z, modulo);
//     if(antimodmin < lb_x) {
//       lb_x = antimodmin;
//     } 
//     antimodmax = max_antimodulo(min_x, min_z, max_z, modulo);
//     if(antimodmax > ub_x) {
//       ub_x = antimodmax;
//     } 

//     if(modmin>max_z || modmax<min_z || antimodmin>max_x || antimodmax<min_x) {
//       // forbidden value
//       if(IS_FAIL(scope[1].remove(modulo))) wiped = FAILURE(1);
//     }
//   }

//   if(IS_OK(wiped) && IS_FAIL(scope[2].set_max(max_z))) wiped = FAILURE(1);
//   if(IS_OK(wiped) && IS_FAIL(scope[2].set_min(min_z))) wiped = FAILURE(1);
//   if(IS_OK(wiped) && IS_FAIL(scope[0].set_min(min_x))) wiped = FAILURE(0);
//   if(IS_OK(wiped) && IS_FAIL(scope[0].set_max(max_x))) wiped = FAILURE(0);



//   // // int ub, lb, modulo, target, k, vali, vnxt;  
//   // // if( changed_idx ) { // prune x0
//   // //   if( scope[1].is_ground() ) { // modulo is known
//   // //     modulo = scope[1].get_min();
//   // //     if( modulo == 1 ) {// special case
//   // // 	if(IS_FAIL(scope[0].set_domain(0))) wiped = FAILURE(0);
//   // //     } else if( scope[2].is_ground() ) { // target is known
//   // // 	target = scope[2].get_value();
//   // // 	if(target >= modulo) wiped = FAILURE(0);
      
//   // // 	if(IS_OK(wiped)) {
//   // // 	  // positive/negative target
//   // // 	  if( target < 0 ) {
//   // // 	    if(IS_FAIL(scope[0].set_max(0))) wiped = FAILURE(0);
//   // // 	  } else if( target > 0 ) {
//   // // 	    if(IS_FAIL(scope[0].set_min(0))) wiped = FAILURE(0);
//   // // 	    //consistent = scope[0]->setMin(0);
//   // // 	  } else { 
//   // // 	    if(IS_FAIL(scope[0].set_domain(0))) wiped = FAILURE(0);
//   // // 	    //consistent = scope[0]->setDomain(0);
//   // // 	  }

//   // // 	  if(IS_OK(wiped)) {
//   // // 	    // remove intervals [target+1+k*modulo..target+k*(modulo+1)-1]
//   // // 	    k = (scope[0].get_max()-target-1)/modulo;
//   // // 	    while(IS_OK(wiped)) {
//   // // 	      lb = (target+1+k*modulo);
//   // // 	      ub = (target+(k+1)*modulo-1);
//   // // 	      if( ub < scope[0].get_min() ) break;
//   // // 	      if(IS_FAIL(scope[0].remove_range( lb, ub ))) wiped = FAILURE(0);
//   // // 	      --k;
//   // // 	    }
//   // // 	  }
//   // // 	}      
//   // //     } else {
//   // // 	// prune x0 with respect to x2
	
//   // // 	vnxt = scope[0].get_min();
//   // // 	vali = vnxt-1;
//   // // 	while(vali<vnxt) {
//   // // 	  vali = vnxt;
//   // // 	  k = (vali % modulo);
//   // // 	  if(!scope[2].contain( k ) && IS_FAIL(scope[0].remove( vali ))) wiped = FAILURE(0);
//   // // 	  vnxt = scope[0].next(vali);
//   // // 	} 
	
//   // //     }
//   // //   } else {
//   // //     // modulo is not known we want to prune x0
//   // //     //[TODO!!]
//   // //   }
//   // // } else if( changed_idx != 2 ) {
//   // //   // prune x2

//   // //   if( scope[1].is_ground() ) { // modulo is known
//   // //     if( scope[0].is_ground() ) {
//   // // 	//consistent = scope[2]->setDomain( (scope[0].get_min() % scope[1].get_min()) );
//   // // 	if(IS_FAIL(scope[2].set_domain( (scope[0].get_value() % scope[1].get_value()) ))) wiped = FAILURE(2);
//   // //     } else {
	
//   // // 	//[TODO: this is wrong (?!!)]
//   // // 	modulo = scope[1].get_value();
//   // // 	ub = scope[0].get_max();
//   // // 	if( ub > 0 && modulo <= ub )
//   // // 	  ub = modulo-1;
//   // // 	else ub = 0;
//   // // 	lb = scope[0].get_min();
//   // // 	if( lb < 0 && 1-modulo > lb )
//   // // 	  lb = 1-modulo;
//   // // 	else lb = 0;
	
//   // // 	if(IS_FAIL(scope[2].set_max(ub))) wiped = FAILURE(2);
//   // // 	else if(IS_FAIL(scope[2].set_min(lb))) wiped = FAILURE(2);
//   // // 	//consistent = scope[2]->setMax( ub ) && scope[2]->setMin( lb );
	
//   // // 	vnxt = scope[2].get_min();
//   // // 	vali = vnxt-1;
//   // // 	while(IS_OK(wiped) && vali < vnxt) {
//   // // 	  vali = vnxt;
	  
//   // // 	  k = vali;
//   // // 	  if( k > 0 ) {
//   // // 	    lb = (scope[0].get_min()/modulo)*modulo;
//   // // 	    k = std::max( k, lb+k );
//   // // 	    while( !scope[0].contain(k) && k <= scope[0].get_max() )  {
//   // // 	      k+=modulo;
//   // // 	    }
//   // // 	    if(k && k > scope[0].get_max()) {
//   // // 	      if(IS_FAIL(scope[2].remove( vali ))) wiped = FAILURE(2);
//   // // 	    } 
//   // // 	  }
//   // // 	  else {
//   // // 	    ub = (scope[0].get_max()/modulo)*modulo;
//   // // 	    if(k) k = std::min( k, ub+k );
//   // // 	    else k = ub;
//   // // 	    while( !scope[0].contain(k) && k >= scope[0].get_min() ) {
//   // // 	      k-=modulo;
//   // // 	    }
//   // // 	    if(k < scope[0].get_min()) {
//   // // 	      if(IS_FAIL(scope[2].remove( vali ))) wiped = FAILURE(2);
//   // // 	    } 
	    
//   // // 	    vnxt = scope[2].next(k);
//   // // 	  }
//   // // 	} 
//   // //     }
//   // //   } else {
//   // //     // modulo is not known we want to prune x2
//   // //   }
//   // // } 
  


// #ifdef _DEBUG_MOD
//   if(_DEBUG_MOD) {
//     std::cout << on[0] << std::endl
// 	      << on[1] << std::endl
// 	      << on[2] << std::endl
// 	      << scope[0].get_domain() << " % " 
// 	      << scope[1].get_domain() << " = " 
// 	      << scope[2].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
//   }
// #endif
  
//   return wiped;
}

std::ostream& Mistral::PredicateMod::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " % " << scope[1]/*.get_var()*/ << ")";
  return os;
}



void Mistral::PredicateCMod::initialise() {
  ConstraintImplementation::initialise();

  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  trigger_on(_RANGE_, scope[2]);
}

void Mistral::PredicateCMod::mark_domain() {
  for(int i=0; i<3; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}


Mistral::PropagationOutcome Mistral::PredicateCMod::rewrite() {
   Mistral::PropagationOutcome wiped = propagate();



  // VarArray tmp;
  // if(active == 3) {
  //   ConstraintImplementation *con;
  //   int i=0;
  //   for(; i<2; ++i)
  //     if(scope[i].is_ground()) {
  // 	relax();
  // 	tmp.add(scope[1-i]);
  // 	tmp.add(scope[2]);
  // 	if(scope[i].get_min() == 0) {
  // 	  con = new ConstraintEqual(tmp);
  // 	} else {
  // 	  con = new PredicateOffset(tmp, scope[i].get_min());
  // 	}
  // 	get_solver()->add(Constraint(con, con->type));
  //     }
  // }
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateCMod::propagate() { 
  if(IS_FAIL(scope[1].remove(0))) return FAILURE(1);
  return filter();
}

Mistral::PropagationOutcome Mistral::PredicateCMod::filter() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;


  //std::cout << "\n ^^^" << std::endl;

#ifdef _DEBUG_CMOD
  if(_DEBUG_CMOD) {
    std::cout << scope[0] << " % " 
	      << scope[1] << " = " 
	      << scope[2] << std::endl; 
    std::cout << scope[0].get_domain() << " % " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << std::endl 
	      << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl;
  }
#endif


  int vnext = scope[1].get_min(), modulo = vnext-1;
  // int minmin_x =  INFTY;
  // int maxmax_x = -INFTY;

  int minmin_y =  INFTY;
  int maxmax_y = -INFTY;

  IntervalList pos_ub;
  IntervalList neg_ub;

  IntervalList llb;

  IntervalList forbidden_intervals;

  IntervalList last_intervals;
  IntervalList cur_intervals;

  forbidden_intervals.push(-INFTY, INFTY);

  while(vnext > modulo) {
    modulo = vnext;


#ifdef _DEBUG_CMOD
    if(_DEBUG_CMOD) {
      std::cout << " -> modulo = " << modulo << std::endl;
  }
#endif

    Interval X(scope[0].get_min(), scope[0].get_max());
    
    Interval cur_Y(scope[2].get_min(), scope[2].get_max());
    
    Interval Y = X.target_c_modulo(modulo, cur_Y);
    
    //std::cout << X << "%" << modulo << " ^ " << cur_Y << " = " << Y << std::endl;
    
#ifdef _DEBUG_CMOD
    if(_DEBUG_CMOD) {
      std::cout << " -> " << X << " % " << modulo << " = " << Y << std::endl;
    }
#endif
    
    if(scope[2].get_min() > Y.max || scope[2].get_max() < Y.min) {
      
      
#ifdef _DEBUG_CMOD
      if(_DEBUG_CMOD) {
	std::cout << " [inconsistent] " << std::endl;
      }
#endif
      
      if(IS_FAIL(scope[1].remove(modulo))) wiped = FAILURE(1);
    } else {
      
#ifdef _DEBUG_CMOD
      if(_DEBUG_CMOD) {
	std::cout << " [ok] " << std::endl;
      }
#endif
            
      if(minmin_y > Y.min) minmin_y = Y.min;
      if(maxmax_y < Y.max) maxmax_y = Y.max;
    }
  
    if(IS_OK(wiped)) {

    // because of Y's lb:
    int min_y = scope[2].get_min();
    int max_y = scope[2].get_max();

    int min_x = scope[0].get_min();
    int max_x = scope[0].get_max();
    
    int lb, ub, incr = abs(modulo);


    llb.clear();
    pos_ub.clear();
    neg_ub.clear();

    
    if(IS_OK(wiped)) {
      
      if(min_y > 0) { // the image of the modulo is positive, so the domain must be positive
	
#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	  std::cout << "   => " << scope[2]  << " in " << scope[2].get_domain() 
		    << " is positive, hence "<< scope[0] << " must be positive " << std::endl;
	}
#endif	  
	
	//if(IS_FAIL(scope[0].set_min(1))) wiped = FAILURE(0);
	
	llb.push(-INFTY, 0);
	
	// we know x is positive
	//min_x = scope[0].get_min();
	if(min_x <= 0) min_x = 1;
	lb = min_x - min_x % modulo;
	ub = lb + min_y - 1;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && lb <= max_x) {
// #ifdef _DEBUG_CMOD
// 	    if(_DEBUG_CMOD) {
// 	      std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
// 	    }
// #endif	
	    //if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    llb.push((lb <= 0 ? 1 : lb), ub);
	    
	    lb += incr;
	    ub += incr;
	  }
	}

      } else if(max_y < 0) { // the image of the modulo is negative, so the domain must be negative
	
	//if(IS_FAIL(scope[0].set_max(0))) wiped = FAILURE(0);
	  
#ifdef _DEBUG_CMOD
	  if(_DEBUG_CMOD) {
	    std::cout << "   => " << scope[1]  << " in " << scope[1].get_domain() 
		      << " is negative, hence "<< scope[0] << " must be negative " << std::endl;
	  }
#endif
	
// 	// we know x is negative
// 	//max_x = scope[0].get_max();
// 	if(max_x >= 0) max_x = -1;
// 	// pruning because of Y's ub on the negative half of X
// 	ub = max_x - max_x % modulo;
// 	lb = ub + max_y + 1;
	
// 	if(lb<=ub) {
// 	  while(IS_OK(wiped) && ub >= min_x) {
// // #ifdef _DEBUG_CMOD
// // 	    if(_DEBUG_CMOD) {
// // 	      std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
// // 	    }
// // #endif	
// 	    //if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
// 	    llb.push(lb, ub));
// 	    lb -= incr;
// 	    ub -= incr;
// 	  }
// 	  //min_x = scope[0].get_min();
// 	  //max_x = scope[0].get_max();
// 	}

	// we know x is negative
	//max_x = scope[0].get_max();
	if(max_x >= 0) max_x = -1;
	// pruning because of Y's ub on the negative half of X
	ub = min_x - min_x % modulo;
	lb = ub + max_y + 1;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && lb <= max_x) {
// #ifdef _DEBUG_CMOD
// 	    if(_DEBUG_CMOD) {
// 	      std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
// 	    }
// #endif	
	    //if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    llb.push(lb, (ub >= 0 ? -1 : ub));
	    lb += incr;
	    ub += incr;
	  }
	  //min_x = scope[0].get_min();
	  //max_x = scope[0].get_max();
	}

	llb.push(0, INFTY);

      } //else { // min(y) < 0 < max(y)
      
      if(max_y >= 0 && max_y < incr-1) {
	//pos_ub.clear();

#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	  std::cout << "   => " << scope[2]  << " in " << scope[2].get_domain() 
		    << " < " << incr << ", hence we prune " << scope[0] << std::endl;
	}
#endif
	
	// pruning because of Y's ub on the positive half of X
	if(min_x > 0) {
	  ub = min_x - min_x % modulo + incr - 1;
	} else {
	  ub = incr-1;
	}
	lb = ub - incr + max_y + 2;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && lb <= max_x) {
// #ifdef _DEBUG_CMOD
// 	    if(_DEBUG_CMOD) {
// 	      std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
// 	    }
// #endif	
	    //if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    pos_ub.push((lb <= 0 ? 1 : lb),ub);
	    lb += incr;
	    ub += incr;
	  }
	  //min_x = scope[0].get_min();
	  //max_x = scope[0].get_max();
	}
      }
      
      if(min_y <= 0 && min_y > 1-incr) {
	//neg_ub.clear();

#ifdef _DEBUG_CMOD
	if(_DEBUG_CMOD) {
	  std::cout << "   => " << scope[2]  << " in " << scope[2].get_domain() 
		    << " > " << -incr << ", hence we prune " << scope[0] << std::endl;
	}
#endif
	
// 	// pruning because of Y's lb on the negative half of X
// 	if(max_x < 0) {
// 	  lb = max_x - max_x % modulo - incr + 1;
// 	} else {
// 	  lb = 1-incr;
// 	}
// 	ub = lb + incr + min_y - 2;
	
// 	if(lb<=ub) {
// 	  while(IS_OK(wiped) && ub >= min_x) {
// // #ifdef _DEBUG_CMOD
// // 	    if(_DEBUG_CMOD) {
// // 	      std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
// // 	    }
// // #endif	
// 	    //if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
// 	    neg_ub.push(lb,ub));
// 	    lb -= incr;
// 	    ub -= incr;
// 	  }
// 	  //min_x = scope[0].get_min();
// 	  //max_x = scope[0].get_max();
// 	}

	// pruning because of Y's lb on the negative half of X
	// if(max_x < 0) {
	//   lb = max_x - max_x % modulo - incr + 1;
	// } else {
	//   lb = 1-incr;
	// }
	lb = min_x - min_x % modulo - incr + 1;
	ub = lb + incr + min_y - 2;
	
	if(lb<=ub) {
	  while(IS_OK(wiped) && lb <= 0) {
#ifdef _DEBUG_CMOD
	    if(_DEBUG_CMOD) {
	      std::cout << "    -> remove interval [" << lb << "," << ub << "]" << std::endl;
	    }
#endif	
	    //if(IS_FAIL(scope[0].remove_interval(lb, ub))) wiped = FAILURE(0);
	    neg_ub.push(lb,(ub >= 0 ? -1 : ub));
	    lb += incr;
	    ub += incr;
	  }
	  //min_x = scope[0].get_min();
	  //max_x = scope[0].get_max();
	}
      } 
    }
    
    
    }
    


#ifdef _DEBUG_CMOD
    if(_DEBUG_CMOD) {
      std::cout << " lb pruning: " << llb << std::endl;
      std::cout << " ub pruning: " << neg_ub << " and " << pos_ub << std::endl;
    }
#endif


    last_intervals = forbidden_intervals;
    cur_intervals.clear();
    forbidden_intervals.clear();

    if(!llb.empty()) {
      if(!pos_ub.empty()) {
	llb.union_with(pos_ub, cur_intervals);
	if(!neg_ub.empty()) {
	  forbidden_intervals = cur_intervals;
	  neg_ub.union_with(forbidden_intervals, cur_intervals);
	}
      } else if(!neg_ub.empty()) {
	llb.union_with(neg_ub, cur_intervals);
      } else {
	cur_intervals = llb;
      }
    } else if(!pos_ub.empty()) {
      if(!neg_ub.empty()) {
	pos_ub.union_with(neg_ub, cur_intervals);
      } else {
	cur_intervals = pos_ub;
      }
    } else if(!neg_ub.empty()) {
      cur_intervals = neg_ub;
    }

    last_intervals.intersect_with(cur_intervals,forbidden_intervals);


#ifdef _DEBUG_CMOD
    if(_DEBUG_CMOD) {
      std::cout << " all pruning for this round: " << cur_intervals << std::endl;
      std::cout << " all pruning: " << forbidden_intervals << std::endl;
    }
#endif
    
    vnext = scope[1].next(modulo);

  }
  
  //if(IS_OK(wiped) && minmin_x <  INFTY && IS_FAIL(scope[0].set_min(minmin_x))) wiped = FAILURE(0);
  if(IS_OK(wiped) && minmin_y <  INFTY && IS_FAIL(scope[2].set_min(minmin_y))) wiped = FAILURE(2);
  //if(IS_OK(wiped) && maxmax_x > -INFTY && IS_FAIL(scope[0].set_max(maxmax_x))) wiped = FAILURE(0);
  if(IS_OK(wiped) && maxmax_y > -INFTY && IS_FAIL(scope[2].set_max(maxmax_y))) wiped = FAILURE(2);
    
 
#ifdef _DEBUG_CMOD
  if(_DEBUG_CMOD) {
    std::cout << " +++> remove " << forbidden_intervals << " from " << scope[0].get_domain() << "\n";
  }
#endif

  for(unsigned int i=0; IS_OK(wiped) && i<forbidden_intervals.size; ++i) {
    if(IS_FAIL(scope[0].remove_interval(forbidden_intervals[i].min, forbidden_intervals[i].max))) wiped = FAILURE(0);
  }

  //}
    // if(IS_FAIL(scope[0].set_domain(minmin_x, maxmax_x))) wiped = FAILURE(0);
    // else if(IS_FAIL(scope[2].set_domain(minmin_y, maxmax_y))) wiped = FAILURE(2);
  

#ifdef _DEBUG_CMOD
  if(_DEBUG_CMOD) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl
	      << scope[0].get_domain() << " % " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif
  

  //std::cout << " vvv" << std::endl;

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateCMod::propagate(const int changed_idx, 
							      const Event evt) {    
  return filter();
}

std::ostream& Mistral::PredicateCMod::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " % " << scope[1]/*.get_var()*/ << ")";
  return os;
}


void Mistral::PredicateDiv::initialise() {
  ConstraintImplementation::initialise();

  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  trigger_on(_RANGE_, scope[2]);
}

void Mistral::PredicateDiv::mark_domain() {
  for(int i=0; i<3; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}



// Mistral::PropagationOutcome Mistral::PredicateDiv::rewrite() {
//    Mistral::PropagationOutcome wiped = propagate();



//   // VarArray tmp;
//   // if(active == 3) {
//   //   ConstraintImplementation *con;
//   //   int i=0;
//   //   for(; i<2; ++i)
//   //     if(scope[i].is_ground()) {
//   // 	relax();
//   // 	tmp.add(scope[1-i]);
//   // 	tmp.add(scope[2]);
//   // 	if(scope[i].get_min() == 0) {
//   // 	  con = new ConstraintEqual(tmp);
//   // 	} else {
//   // 	  con = new PredicateOffset(tmp, scope[i].get_min());
//   // 	}
//   // 	get_solver()->add(Constraint(con, con->type));
//   //     }
//   // }
//   return wiped;
// }

Mistral::PropagationOutcome Mistral::PredicateDiv::propagate() { 
  if(IS_FAIL(scope[1].remove(0))) return FAILURE(1);
  return filter();
}

Mistral::PropagationOutcome Mistral::PredicateDiv::filter() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;


#ifdef _DEBUG_DIV
  if(_DEBUG_DIV) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl
	      << scope[0].get_domain() << " / " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << std::endl;
  std::cout << "filter Z = X/Y" << std::endl;
 }
#endif

  BiInterval X(scope[0]);
  BiInterval Y(scope[1]);
  BiInterval Z = X.divided_by(Y, scope[2]);
  if(IS_FAIL(scope[2].set_domain(Z))) wiped = FAILURE(2);

  
  if(IS_OK(wiped)) {

#ifdef _DEBUG_DIV
  if(_DEBUG_DIV) {
    std::cout << "filter X = Z*Y" << std::endl;
  }
#endif

    Z = BiInterval(scope[2]);
    X = Z.anti_div_X(Y);
    if(IS_FAIL(scope[0].set_domain(X))) wiped = FAILURE(0);

    if(IS_OK(wiped)) {


      X = BiInterval(scope[0]);

#ifdef _DEBUG_DIV
  if(_DEBUG_DIV) {
      std::cout << "X = " << scope[0].get_domain() << std::endl;
      std::cout << "filter Y = X/Z" << std::endl;
      std::cout << "filter " << scope[1].get_domain() << " = " << X << "/" << Z << std::endl;
  }
#endif

      Y = Z.anti_div_Y(X);
      //Y = X.anti_mul(Z);
      if(IS_FAIL(scope[1].set_domain(Y))) wiped = FAILURE(1);

    }
  }



#ifdef _DEBUG_DIV
  if(_DEBUG_DIV) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << on[2] << std::endl
	      << scope[0].get_domain() << " / " 
	      << scope[1].get_domain() << " = " 
	      << scope[2].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif
  

  //std::cout << " vvv" << std::endl;

  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateDiv::propagate(const int changed_idx, 
							     const Event evt) {    

 return filter();
}

std::ostream& Mistral::PredicateDiv::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " / " << scope[1]/*.get_var()*/ << ")";
  return os;
}



void Mistral::PredicateDivConstant::initialise() {
  ConstraintImplementation::initialise();
  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
}

void Mistral::PredicateDivConstant::mark_domain() {
  for(int i=0; i<2; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}



Mistral::PropagationOutcome Mistral::PredicateDivConstant::filter() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;

#ifdef _DEBUG_DIV
  if(_DEBUG_DIV) {
    std::cout << scope[0] << " / " 
	      << quotient << " = " 
	      << scope[1] << std::endl; 
    std::cout << scope[0].get_domain() << " / " 
	      << quotient << " = " 
	      << scope[1].get_domain() << std::endl 
	      << on[0] << std::endl
	      << on[1] << std::endl;
  }
#endif

  BiInterval X(scope[0]);

  BiInterval Y = X/quotient;
  

  if(IS_FAIL(scope[1].set_domain(Y))) wiped = FAILURE(1);

  
  if(IS_OK(wiped)) {

    Y = BiInterval(scope[1]);

    X = Y.anti_div_X(quotient);

    if(IS_FAIL(scope[0].set_domain(X))) wiped = FAILURE(0);

  }


#ifdef _DEBUG_DIV
  if(_DEBUG_DIV) {
    std::cout << on[0] << std::endl
	      << on[1] << std::endl
	      << scope[0].get_domain() << " / " 
	      << quotient << " = " 
	      << scope[1].get_domain() << (IS_OK(wiped) ? " ok" : " fail!") << std::endl << std::endl; 
  }
#endif
  
  return wiped;
}

Mistral::PropagationOutcome Mistral::PredicateDivConstant::propagate() {      
  return filter();
}

Mistral::PropagationOutcome Mistral::PredicateDivConstant::propagate(const int changed_idx, const Event evt) {
  return filter();
}

std::ostream& Mistral::PredicateDivConstant::display(std::ostream& os) const {
  os << scope[1]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " / " << quotient << ")";
  return os;
}



void Mistral::PredicateMul::initialise() {
  ConstraintImplementation::initialise();

  trigger_on(_RANGE_, scope[0]);
  trigger_on(_RANGE_, scope[1]);
  trigger_on(_RANGE_, scope[2]);

  GlobalConstraint::initialise();

  enforce_nfc1 = false;
}

void Mistral::PredicateMul::mark_domain() {
  for(int i=0; i<3; ++i)
    get_solver()->forbid(scope[i].id(), LIST_VAR);
}


Mistral::PropagationOutcome Mistral::PredicateMul::rewrite() {
  // TODO: recode that
  
Mistral::PropagationOutcome wiped = propagate();

  VarArray tmp;
  if(active.size == 2) {
    int i=0;
    for(; i<2; ++i)
      if(scope[i].is_ground()) {
	relax();
	tmp.add(scope[1-i]);
	tmp.add(scope[2]);
	if(scope[i].get_min() == 1) {
	  get_solver()->add(Constraint(new ConstraintEqual(tmp)));
	} else if(scope[i].get_min() != 0) {
	  get_solver()->add(Constraint(new PredicateFactor(tmp, scope[i].get_min())));
	}
      }
  }
  return wiped;
}

inline int xtimey( const int x, const int y, int& r )
{
  r = 0 ;

#ifdef _DEBUG_MUL
  std::cout << x << " * " << y << " = " << (x*y) << std::endl;
#endif

  return (x*y);
}

inline int xovery( const int x, const int y, int& r )
{

#ifdef _DEBUG_MUL
  std::cout << x << " / " << y << " = " ;
#endif

  if(y) {
    r = (x%y != 0);

#ifdef _DEBUG_MUL
    std::cout << (x/y) << std::endl;
#endif

    return x/y;
  }

#ifdef _DEBUG_MUL
    std::cout << (x<0 ? -INFTY : INFTY) << std::endl;
#endif

  return (x<0 ? -INFTY : INFTY);
}

inline int yoverx( const int x, const int y, int& r )
{

#ifdef _DEBUG_MUL
  std::cout << y << " / " << x << " = " ;
#endif

  if(x) {
    r = -(y%x != 0);

#ifdef _DEBUG_MUL
    std::cout << (y/x) << std::endl;
#endif

    return y/x;
  }

#ifdef _DEBUG_MUL
    std::cout << (y<0 ? -INFTY : INFTY) << std::endl;
#endif

  return (y<0 ? -INFTY : INFTY);
}



Mistral::PropagationOutcome Mistral::PredicateMul::revise_division(const int X, const int Y, const int Z) {
  // revise the domain of Z = X/Y (because Z*Y = X)
  Mistral::PropagationOutcome wiped = CONSISTENT;


#ifdef _DEBUG_MUL
  if(_DEBUG_MUL) {

  std::cout << "revise bounds of " << scope[Z].get_domain() << " = " 
	    << scope[X].get_domain() << "/" << scope[Y].get_domain() 
	    << " = [" << min_neg[Z] << ".." << max_neg[Z] << "|" 
	    << (zero[Z] ? "{0}" : "_") << "|" 
	    << min_pos[Z] << ".." << max_pos[Z] << "]" 
	    << std::endl; 
 
  }
#endif

  
  //int lb_pos=1, ub_pos=+INFTY, lb_neg=-INFTY, ub_neg=-1, lb_aux, ub_aux;
  
  // we start with all bounds at their previous values, and update them if necessary
  // (in which case we set up the pruning flag)
  int lb_pos=min_pos[Z], ub_pos=max_pos[Z], lb_neg=min_neg[Z], ub_neg=max_neg[Z], 
    nlb1, nlb2, nub1, nub2;
    //lb_aux, ub_aux;
  bool pruning_flag = false, pzero = false, ppos = max_pos[Z]<=0, pneg = min_neg[Z]>=0;
  
  // first rule: if X can be 0, and Y can be 0, then Z can be anything
  if(!zero[X]) { // if X cannot be 0, then neither can Y nor Z
    if(zero[Z]) {
      //zero[Z] = 0;
      pzero = true;
      pruning_flag = true;
    }
  } else { 
    if(!min_neg[X] && !max_pos[X] && !zero[Y]) {
      // if X must be 0 and Y cannot be 0, then Z must be 0.
      if(lb_neg<0 || ub_pos>0) {
	lb_neg = 0;
	ub_pos = 0;
	pruning_flag = true;
      }
    } 
  }

  if(lb_neg != ub_pos && (!zero[X] || !zero[Y])) { // if X and Y can both be 0, we cannot deduce anything
    if(IS_OK(wiped)) {
      if(max_pos[Z]>0) {
	// revise the positive part of Z's domain (if it has one)
	nlb1 = nlb2 = INFTY; //lb_neg;
	nub1 = nub2 = 0; //ub_neg;
	
	// it can either be the positive parts of X and Y:
	if(max_pos[X]>0 && max_pos[Y]>0) {
	  // compute the bounds
	  nlb1 = (int)(ceil((double)(min_pos[X])/(double)(max_pos[Y])));
	  nub1 = (int)(floor((double)(max_pos[X])/(double)(min_pos[Y])));
	}

	// or the negative parts of X and Y:
	if(min_neg[X]<0 && min_neg[Y]<0) {
	  // compute the bounds
	  nlb2 = (int)(ceil((double)(max_neg[X])/(double)(min_neg[Y])));
	  nub2 = (int)(floor((double)(min_neg[X])/(double)(max_neg[Y])));
	}
	if(nlb1>nlb2) nlb1 = nlb2;
	if(nub1<nub2) nub1 = nub2;

	if(lb_pos<nlb1) {
	  lb_pos = nlb1;
	  pruning_flag = true;
	}
	if(ub_pos>nub1) {
	  ub_pos = nub1;
	  pruning_flag = true;
	}

	if(lb_pos > max_pos[Z] || ub_pos < min_pos[Z]) ppos = true;

      } else if(pzero || !zero[Z]) // if(lb_pos || ub_pos)
	{
	  lb_pos = min_neg[Z];
	  ub_pos = max_neg[Z];
	} else {
	lb_pos = ub_pos = 0; 
      }

     if(min_neg[Z]<0) {
       // revise the negative part of Z's domain (if it has one)
       nlb1 = nlb2 = 0; //lb_pos;
       nub1 = nub2 = -INFTY; //ub_pos;
	
	// it can either be the negitive part of X and the positive part of Y:
	if(min_neg[X]<0 && max_pos[Y]>0) {
	  // compute the bounds

	  nlb1 = (int)(ceil((double)(min_neg[X])/(double)(min_pos[Y])));
	  nub1 = (int)(floor((double)(max_neg[X])/(double)(max_pos[Y])));
	}
	// or the negitive part of Y and the positive part of X:
	if(max_pos[X]>0 && min_neg[Y]<0) {
	  // compute the bounds
	  nlb2 = (int)(ceil((double)(max_pos[X])/(double)(max_neg[Y])));
	  nub2 = (int)(floor((double)(min_pos[X])/(double)(min_neg[Y])));
	}

	if(nlb1>nlb2) nlb1 = nlb2;
	if(nub1<nub2) nub1 = nub2;

	if(lb_neg<nlb1) {
	  lb_neg = nlb1;
	  pruning_flag = true;
	}
	if(ub_neg>nub1) {
	  ub_neg = nub1;
	  pruning_flag = true;
	}

	if(lb_neg > max_neg[Z] || ub_neg < min_neg[Z]) pneg = true;

     } else if(pzero || !zero[Z])// if(lb_neg || ub_neg)
       {
	 lb_neg = min_pos[Z];
	 ub_neg = max_pos[Z];
       } else {
       lb_neg = ub_neg = 0;
     }
    }
  }

  if(pneg && (pzero || !zero[Z]) && ppos) {
    wiped = FAILURE(Z);
  } else if(pruning_flag) {
    
#ifdef _DEBUG_MUL
  if(_DEBUG_MUL) {

    std::cout << "set bounds to " // << scope[Z].get_domain() << " = " 
	      << "[" << lb_neg << ".." << ub_neg << "|" 
	      << (!pzero&&zero[Z] ? "{0}" : "_") << "|" 
	      << lb_pos << ".." << ub_pos << "]" << std::endl; 
 
  }
#endif
    
    wiped = prune(lb_neg, ub_neg, lb_pos, ub_pos, pzero, Z);
  }

  return wiped;
}


Mistral::PropagationOutcome Mistral::PredicateMul::revise_multiplication(const int X, const int Y, const int Z) {
  // revise the domain of Z = X*Y 
  Mistral::PropagationOutcome wiped = CONSISTENT;


#ifdef _DEBUG_MUL
  if(_DEBUG_MUL) {

  std::cout << "revise bounds of " << scope[Z].get_domain() << " = " 
	    << scope[X].get_domain() << "*" << scope[Y].get_domain() 
	    << std::endl; 
 
  }
#endif

  int lb_pos=min_pos[Z], ub_pos=max_pos[Z], lb_neg=min_neg[Z], ub_neg=max_neg[Z], 
    nlb1, nlb2, nub1, nub2;
  
  bool pruning_flag = false, pzero = false;
  
  // if X = 0 or Y = 0, then Z = 0
  if( zero[Z] &&
      !zero[X] && !zero[Y]) {
    //zero[Z] = 0;
    pzero = true;
    pruning_flag = true;
  }
  else if((!min_neg[X] && !max_pos[X]) || (!min_neg[Y] && !max_pos[Y])) { 
    lb_neg = 0;
    ub_pos = 0;
    pruning_flag = true;
  }

  if(lb_neg != ub_pos) { 
    if(IS_OK(wiped)) {
      if(max_pos[Z]>0) {
	// revise the positive part of Z's domain (if it has one)
	nlb1 = nlb2 = INFTY; //lb_neg;
	nub1 = nub2 = 0; //ub_neg;

	// it can either be the positive parts of X and Y:
	if(max_pos[X]>0 && max_pos[Y]>0) {
	  // compute the bounds
	  nub1 = max_pos[X] * max_pos[Y];
	  nlb1 = min_pos[X] * min_pos[Y];
	}
	// or the negative parts of X and Y:
	if(min_neg[X]<0 && min_neg[Y]<0) {
	  // compute the bounds
	  nub2 = min_neg[X] * min_neg[Y];
	  nlb2 = max_neg[X] * max_neg[Y];
	}
	if(nlb1>nlb2) nlb1 = nlb2;
	if(nub1<nub2) nub1 = nub2;

	if(lb_pos<nlb1) {
	  lb_pos = nlb1;
	  pruning_flag = true;
	}
	if(ub_pos>nub1) {
	  ub_pos = nub1;
	  pruning_flag = true;
	}
      } else if(pzero || !zero[Z]) {
	lb_pos = min_neg[Z];
	ub_pos = max_neg[Z];
      } else {
	lb_pos = ub_pos = 0;
      }

     if(min_neg[Z]<0) {
       // revise the negative part of Z's domain (if it has one)
       nlb1 = nlb2 = 0; //lb_pos;
       nub1 = nub2 = -INFTY; //ub_pos;
	
	// it can either be the negitive part of X and the positive part of Y:
	if(min_neg[X]<0 && max_pos[Y]>0) {
	  // compute the bounds
	  nub1 = max_neg[X] * min_pos[Y];
	  nlb1 = min_neg[X] * max_pos[Y];
	}
	// or the negitive part of Y and the positive part of X:
	if(max_pos[X]>0 && min_neg[Y]<0) {
	  // compute the bounds
	  nub2 = max_neg[Y] * min_pos[X];
	  nlb2 = min_neg[Y] * max_pos[X];
	}

	if(nlb1>nlb2) nlb1 = nlb2;
	if(nub1<nub2) nub1 = nub2;
	
	if(lb_neg<nlb1) {
	  lb_neg = nlb1;
	  pruning_flag = true;
	}
	if(ub_neg>nub1) {
	  ub_neg = nub1;
	  pruning_flag = true;
	}
     }  else if(pzero || !zero[Z]) {
       lb_neg = min_pos[Z];
       ub_neg = max_pos[Z];
     } else {
       lb_neg = ub_neg = 0;
     }
    }
  }

  if(pruning_flag) {
#ifdef _DEBUG_MUL
  if(_DEBUG_MUL) {

  std::cout << "set bounds to " // << scope[Z].get_domain() << " = " 
	    << "[" << lb_neg << ".." << ub_neg << "|" 
	    << (zero[Z] ? "{0}" : "_") << "|" 
	    << lb_pos << ".." << ub_pos << "]" << std::endl; 
 
  }
#endif
  
    wiped = prune(lb_neg, ub_neg, lb_pos, ub_pos, pzero, Z);
  }
  
  return wiped;
}


Mistral::PropagationOutcome Mistral::PredicateMul::prune(const int lb_neg, 
							 const int ub_neg, 
							 const int lb_pos, 
							 const int ub_pos,
							 const bool pzero,
							 const int Z) {

  Event evt;
  Mistral::PropagationOutcome wiped = CONSISTENT;
  
  if(ub_pos < lb_neg) wiped = FAILURE(Z);
  else {
    if(lb_neg>min_neg[Z]) {
      evt = scope[Z].set_min( lb_neg );
      if( IS_FAIL(evt) ) wiped = FAILURE(Z);
      else {
	if(changes.contain(Z)) {
	  event_type[Z] |= evt;
	} else {
	  event_type[Z] = evt;
	  changes.add(Z);
	}
	min_neg[Z] = scope[Z].get_min();
      }
    }
    if(IS_OK(wiped) && ub_pos<max_pos[Z]) {
      evt = scope[Z].set_max( ub_pos );
      if( IS_FAIL(evt) ) wiped = FAILURE(Z);
      else {
	if(changes.contain(Z)) {
	  event_type[Z] |= evt;
	} else {
	  event_type[Z] = evt;
	  changes.add(Z);
	}
	max_pos[Z] = scope[Z].get_max();
      }
    }
    if(IS_OK(wiped) && (lb_pos>=min_neg[Z] || ub_neg<=max_pos[Z])) { 
      if(lb_pos-1>ub_neg && (pzero || (!zero[Z] && (min_pos[Z]<lb_pos || max_neg[Z]>ub_neg)))) {
	evt = scope[Z].remove_interval(ub_neg+1, lb_pos-1);
	if( IS_FAIL(evt) ) wiped = FAILURE(Z);
	else {
	  if(changes.contain(Z)) {
	    event_type[Z] |= evt;
	  } else {
	    event_type[Z] = evt;
	    changes.add(Z);
	  }
	  zero[Z] = 0;
	  min_pos[Z] = scope[Z].get_min_pos();
	  max_neg[Z] = scope[Z].get_max_neg();
	}
      } else {
	if(lb_pos>1 && min_pos[Z]<lb_pos) {
	  evt = scope[Z].remove_interval(1, lb_pos-1);
	  if( IS_FAIL(evt) ) wiped = FAILURE(Z);
	  else {
	    if(changes.contain(Z)) {
	      event_type[Z] |= evt;
	    } else {
	      event_type[Z] = evt;
	      changes.add(Z);
	    }
	    min_pos[Z] = scope[Z].get_min_pos();
	  }
	}
	if(ub_neg<-1 && max_neg[Z]>ub_neg) {
	  evt = scope[Z].remove_interval(ub_neg+1, -1);
	  if( IS_FAIL(evt) ) wiped = FAILURE(Z);
	  else {
	    if(changes.contain(Z)) {
	      event_type[Z] |= evt;
	    } else {
	      event_type[Z] = evt;
	      changes.add(Z);
	    }
	    max_neg[Z] = scope[Z].get_max_neg();
	  }
	}
      }
    }
  }

  if((min_neg[Z]>0 && min_neg[Z]<min_pos[Z]) ||
     (!min_neg[Z] && !zero[Z]))
    min_neg[Z] = min_pos[Z];

  if((max_pos[Z]<0 && max_pos[Z]>max_neg[Z]) ||
     (!max_pos[Z] && !zero[Z]))
    max_pos[Z] = max_neg[Z];


#ifdef _DEBUG_MUL
  if(_DEBUG_MUL) {

    if(IS_OK(wiped)) {
      std::cout << scope[0].get_domain() 
		<< " * " << scope[1].get_domain() 
		<< " = " << scope[2].get_domain() << std::endl;
    } else std::cout << "FAIL!" << std::endl ;
    
    std::cout
      << " now in [" << min_neg[Z] << ".." << max_neg[Z] << "|" 
      << (zero[Z] ? "{0}" : "_") << "|" 
      << min_pos[Z] << ".." << max_pos[Z] << "]" << std::endl;
    
    //std::cout << std::endl; 
    
  }
#endif


  return wiped;
}




Mistral::PropagationOutcome Mistral::PredicateMul::propagate() {      
  Mistral::PropagationOutcome wiped = CONSISTENT;
  
#ifdef _DEBUG_MUL
  if(_DEBUG_MUL) {

  std::cout << std::endl << std::endl << "propagate " // << this 
	    << std::endl << scope[0].get_domain() 
 	    << " * " << scope[1].get_domain() 
     	    << " = " << scope[2].get_domain() << std::endl;
 
  }
#endif

  bool is_ground = true;
  int evt_idx, i;

  for(i=0; i<3; ++i) {
    max_pos[i] = scope[i].get_max();
    min_neg[i] = scope[i].get_min();

    is_ground &= (min_neg[i] == max_pos[i]);

    zero[i] = scope[i].contain(0);

    if(min_neg[i]<0)
      max_neg[i] = scope[i].get_max_neg();
    else
      max_neg[i] = min_neg[i];

    if(max_pos[i]>0)
      min_pos[i] = scope[i].get_min_pos();
    else
      min_pos[i] = max_pos[i];

  }

  
  // #ifdef _DEBUG_MUL
  //   if(_DEBUG_MUL) {
  
  //   std::cout << scope[0]/*.get_var()*/ << " in " << scope[0].get_domain() 
  // 	    << " * " << scope[1]/*.get_var()*/ << " in " << scope[1].get_domain() 
  //     	    << " = " << scope[2]/*.get_var()*/ << " in " << scope[2].get_domain() << std::endl;
  
  //   }
  // #endif
    
  
  if(!is_ground) {
    while(IS_OK(wiped) && !changes.empty()) {
      
      evt_idx = changes.pop();
      
#ifdef _DEBUG_MUL
      if(_DEBUG_MUL) {
	
	std::cout << std::endl << "react to " << scope[evt_idx]/*.get_var()*/ << " in " 
		  << scope[evt_idx].get_domain() 
		  << (LB_CHANGED(event_type[evt_idx]) ? " (change on LB) " : "")
		  << (UB_CHANGED(event_type[evt_idx]) ? " (change on UB) " : "")
		  << std::endl;
	
      }
#endif
      
      // x0 * x1 = x2 
      if(evt_idx == 2) {
	for(int i=0; IS_OK(wiped) && i<2; ++i)  // once for scope[0], once for scope[1]
	  // we update x0 and x1 :  xi = x2/x1-i
	  wiped = revise_division(2, 1-i, i);
      } else {
	// update x[2]
	wiped = revise_multiplication(evt_idx, 1-evt_idx, 2);
	// update x[1-evt_idx]
	if(IS_OK(wiped)) wiped = revise_division(2, evt_idx, 1-evt_idx);
      }
    }

  } else {
    
    if(min_neg[0] * min_neg[1] != min_neg[2]) wiped = 0;

  }

#ifdef _DEBUG_MUL
  if(_DEBUG_MUL) {

    std::cout << std::endl;
    if(IS_OK(wiped)) {
      
      std::cout << "  ===> " << scope[0]/*.get_var()*/ << " in " << scope[0].get_domain() 
		<< " * " << scope[1]/*.get_var()*/ << " in " << scope[1].get_domain() 
		<< " = " << scope[2]/*.get_var()*/ << " in " << scope[2].get_domain() << std::endl;

    } else {

      std::cout << "  ===> INCONSISTENT!!" << std::endl;

    }
 
  }
#endif
  
  return wiped;
}
  
std::ostream& Mistral::PredicateMul::display(std::ostream& os) const {
  os << scope[2]/*.get_var()*/ << " == (" << scope[0]/*.get_var()*/ << " * " << scope[1]/*.get_var()*/ << ")";
  return os;
}



// void Mistral::PredicateDiv::initialise() {
//   Constraint::initialise();
//   trigger_on(_RANGE_, scope[0]);
//   trigger_on(_RANGE_, scope[1]);
//   trigger_on(_RANGE_, scope[2]);
//   set_idempotent(true);
// }

// Mistral::PropagationOutcome Mistral::PredicateDiv::rewrite() {
//   Mistral::PropagationOutcome wiped = propagate();

//   VarArray tmp;
//   if(active.size == 2) {
//     int i=0;
//     for(; i<2; ++i)
//       if(scope[i].is_ground()) {
// 	relax();
// 	tmp.add(scope[1-i]);
// 	tmp.add(scope[2]);
// 	if(scope[i].get_min() == 1) {
// 	  solver->add(new ConstraintEqual(tmp));
// 	} else if(scope[i].get_min() != 0) {
// 	  solver->add(new PredicateFactor(tmp, scope[i].get_min()));
// 	}
//       }
//   }
//   return wiped;
// }




// Mistral::PropagationOutcome Mistral::PredicateDiv::revise_integer_division(const int X, const int Y, const int Z) {
//   // revise the domain of Z = X/Y (Z is the integer part of X/Y)
//   Mistral::PropagationOutcome wiped = CONSISTENT;


// #ifdef _DEBUG_DIV
//   std::cout << "revise bounds of " << scope[Z].get_domain() << " = " 
// 	    << scope[X].get_domain() << "/" << scope[Y].get_domain() 
// 	    << " = [" << min_neg[Z] << ".." << max_neg[Z] << "|" 
// 	    << (zero[Z] ? "{0}" : "_") << "|" 
// 	    << min_pos[Z] << ".." << max_pos[Z] << "]" 
// 	    << std::endl; 
// #endif

  
//   //int lb_pos=1, ub_pos=+INFTY, lb_neg=-INFTY, ub_neg=-1, lb_aux, ub_aux;
  
//   // we start with all bounds at their previous values, and update them if necessary
//   // (in which case we set up the pruning flag)
//   int lb_pos=min_pos[Z], ub_pos=max_pos[Z], lb_neg=min_neg[Z], ub_neg=max_neg[Z], 
//     nlb1, nlb2, nub1, nub2;
//     //lb_aux, ub_aux;
//   bool pruning_flag = false, pzero = false, ppos = max_pos[Z]<=0, pneg = min_neg[Z]>=0;
  
//   // // first rule: if X can be 0, and Y can be 0, then Z can be anything
//   // if(!zero[X]) { // if X cannot be 0, then neither can Y nor Z
//   //   if(zero[Z]) {
//   //     //zero[Z] = 0;
//   //     pzero = true;
//   //     pruning_flag = true;
//   //   }
//   // } else { 
//   //   if(!min_neg[X] && !max_pos[X] && !zero[Y]) {
//   //     // if X must be 0 and Y cannot be 0, then Z must be 0.
//   //     if(lb_neg<0 || ub_pos>0) {
//   // 	lb_neg = 0;
//   // 	ub_pos = 0;
//   // 	pruning_flag = true;
//   //     }
//   //   } 
//   // }

//   //	std::cout << pruning_flag << std::endl;

//   if(lb_neg != ub_pos && (!zero[X] || !zero[Y])) { // if X and Y can both be 0, we cannot deduce anything
//     if(IS_OK(wiped)) {
//       if(max_pos[Z]>0) {
// 	// revise the positive part of Z's domain (if it has one)
// 	nlb1 = nlb2 = INFTY; //lb_neg;
// 	nub1 = nub2 = 0; //ub_neg;
	
// 	// it can either be the positive parts of X and Y:
// 	if(max_pos[X]>0 && max_pos[Y]>0) {
// 	  // compute the bounds
// 	  //std::cout << "\t   lb = " << min_pos[X] << "/" << max_pos[Y] << std::endl;
// 	  //std::cout << "\t   ub = " << max_pos[X] << "/" << min_pos[Y] << std::endl;
// 	  nlb1 = min_pos[X]/max_pos[Y];
// 	  nub1 = max_pos[X]/min_pos[Y];
// 	}

// 	// or the negative parts of X and Y:
// 	if(min_neg[X]<0 && min_neg[Y]<0) {
// 	  // compute the bounds
// 	  nlb2 = max_neg[X]/min_neg[Y];
// 	  nub2 = min_neg[X]/max_neg[Y];
// 	}
// 	if(nlb1>nlb2) nlb1 = nlb2;
// 	if(nub1<nub2) nub1 = nub2;
	
// 	//std::cout << "positive bounds: [" << nlb1 << ".." << nub1 << "]" << std::endl;

// 	if(lb_pos<nlb1) {
// 	  lb_pos = nlb1;
// 	  pruning_flag = true;
// 	}
// 	if(ub_pos>nub1) {
// 	  ub_pos = nub1;
// 	  pruning_flag = true;
// 	}

// 	if(lb_pos > max_pos[Z] || ub_pos < min_pos[Z]) ppos = true;

//       } // else if(pzero || !zero[Z]) // if(lb_pos || ub_pos)
//       // 	{
//       // 	  lb_pos = min_neg[Z];
//       // 	  ub_pos = max_neg[Z];
//       // 	} else {
//       // 	lb_pos = ub_pos = 0; 
//       // }

//      if(min_neg[Z]<0) {
//        // revise the negative part of Z's domain (if it has one)
//        nlb1 = nlb2 = 0; //lb_pos;
//        nub1 = nub2 = -INFTY; //ub_pos;
	
// 	// it can either be the negitive part of X and the positive part of Y:
// 	if(min_neg[X]<0 && max_pos[Y]>0) {
// 	  // compute the bounds

// 	  nlb1 = min_neg[X]/min_pos[Y];
// 	  nub1 = max_neg[X]/max_pos[Y];
// 	}
// 	// or the negitive part of Y and the positive part of X:
// 	if(max_pos[X]>0 && min_neg[Y]<0) {
// 	  // compute the bounds
// 	  nlb2 = max_pos[X]/max_neg[Y];
// 	  nub2 = min_pos[X]/min_neg[Y];
// 	}

// 	if(nlb1>nlb2) nlb1 = nlb2;
// 	if(nub1<nub2) nub1 = nub2;

// 	//std::cout << "negative bounds: [" << nlb1 << ".." << nub1 << "]" << std::endl;
	
// 	if(lb_neg<nlb1) {
// 	  lb_neg = nlb1;
// 	  pruning_flag = true;
// 	}
// 	if(ub_neg>nub1) {
// 	  ub_neg = nub1;
// 	  pruning_flag = true;
// 	}

// 	if(lb_neg > max_neg[Z] || ub_neg < min_neg[Z]) pneg = true;

//      } // else if(pzero || !zero[Z])// if(lb_neg || ub_neg)
//      //   {
//      // 	 lb_neg = min_pos[Z];
//      // 	 ub_neg = max_pos[Z];
//      //   } else {
//      //   lb_neg = ub_neg = 0;
//      // }
//     }
//   }
  
//   //std::cout << pneg <<  (pzero || !zero[Z]) <<  ppos << std::endl;

//   if(pneg && (pzero || !zero[Z]) && ppos) {
//     wiped = FAILURE(Z);
//   } else if(pruning_flag) {
    
// #ifdef _DEBUG_DIV
//     std::cout << "set bounds to " // << scope[Z].get_domain() << " = " 
// 	      // << scope[X].get_domain() << "/" << scope[Y].get_domain() 
// 	      << "[" << lb_neg << ".." << ub_neg << "|" 
// 	      << (!pzero&&zero[Z] ? "{0}" : "_") << "|" 
// 	      << lb_pos << ".." << ub_pos << "]" << std::endl; 
// #endif
    
//     wiped = prune(lb_neg, ub_neg, lb_pos, ub_pos, pzero, Z);
//   }

//   return wiped;
// }


// Mistral::PropagationOutcome Mistral::PredicateDiv::revise_reverse_division(const int X, const int Y, const int Z) {
//   // revise the domain of Z = X/Y (Z is the integer part of X/Y)
//   Mistral::PropagationOutcome wiped = CONSISTENT;


// #ifdef _DEBUG_DIV
//   std::cout << "revise bounds of " << scope[Z].get_domain() << " = " 
// 	    << scope[X].get_domain() << "/" << scope[Y].get_domain() 
// 	    << " = [" << min_neg[Z] << ".." << max_neg[Z] << "|" 
// 	    << (zero[Z] ? "{0}" : "_") << "|" 
// 	    << min_pos[Z] << ".." << max_pos[Z] << "]" 
// 	    << std::endl; 
// #endif

  
//   //int lb_pos=1, ub_pos=+INFTY, lb_neg=-INFTY, ub_neg=-1, lb_aux, ub_aux;
  
//   // we start with all bounds at their previous values, and update them if necessary
//   // (in which case we set up the pruning flag)
//   int lb_pos=min_pos[Z], ub_pos=max_pos[Z], lb_neg=min_neg[Z], ub_neg=max_neg[Z], 
//     nlb1, nlb2, nub1, nub2, aux;
//     //lb_aux, ub_aux;
//   bool pruning_flag = false, pzero = false, ppos = max_pos[Z]<=0, pneg = min_neg[Z]>=0;
  
//   // // first rule: if X can be 0, and Y can be 0, then Z can be anything
//   // if(!zero[X]) { // if X cannot be 0, then neither can Y nor Z
//   //   if(zero[Z]) {
//   //     //zero[Z] = 0;
//   //     pzero = true;
//   //     pruning_flag = true;
//   //   }
//   // } else { 
//   //   if(!min_neg[X] && !max_pos[X] && !zero[Y]) {
//   //     // if X must be 0 and Y cannot be 0, then Z must be 0.
//   //     if(lb_neg<0 || ub_pos>0) {
//   // 	lb_neg = 0;
//   // 	ub_pos = 0;
//   // 	pruning_flag = true;
//   //     }
//   //   } 
//   // }

//   //	std::cout << pruning_flag << std::endl;

//   if(lb_neg != ub_pos && (!zero[X] || !zero[Y])) { // if X and Y can both be 0, we cannot deduce anything
//     if(IS_OK(wiped)) {
//       if(max_pos[Z]>0) {
// 	// revise the positive part of Z's domain (if it has one)
// 	nlb1 = nlb2 = INFTY; //lb_neg;
// 	nub1 = nub2 = 0; //ub_neg;
	
// 	// it can either be the positive parts of X and Y:
// 	if(max_pos[X]>0 && max_pos[Y]>0) {
// 	  // compute the bounds
// 	  //std::cout << "\t   lb = " << min_pos[X] << "/" << max_pos[Y] << std::endl;
// 	  //std::cout << "\t   ub = " << max_pos[X] << "/" << min_pos[Y] << std::endl;
// 	  //if(max_pos[Y] == 1) nlb1 = min_pos[X];
// 	  //else {
// 	  nlb1 = (min_pos[X]/(max_pos[Y]+1));
// 	  aux = ((min_pos[X]-1)/max_pos[Y]);
// 	  if(aux>nlb1) nlb1=aux;
// 	  //}
	  
// 	  if(min_pos[Y] == 1) {
// 	    nub1 = max_pos[X];
// 	  } else {
// 	    nub1 = (max_pos[X]/(min_pos[Y]-1));
// 	    aux = ((max_pos[X]+1)/min_pos[Y]);
// 	    if(aux<nub1) nub1=aux;
// 	  }
// 	}

// 	// or the negative parts of X and Y:
// 	if(min_neg[X]<0 && min_neg[Y]<0) {
// 	  // compute the bounds
// 	  nlb2 = max_neg[X]/(min_neg[Y]-1);
// 	  aux = (max_neg[X]+1)/min_neg[Y];
// 	  if(aux>nlb2) nlb2=aux;

// 	  if(max_neg[Y] == -1) {
// 	    nub2 = -min_neg[X];
// 	  } else {
// 	    nub2 = min_neg[X]/(max_neg[Y]+1);
// 	    aux = (min_neg[X]-1)/max_neg[Y];
// 	    if(aux<nub2) nub2=aux;
// 	  }
// 	}

// 	if(nlb1>nlb2) nlb1 = nlb2;
// 	if(nub1<nub2) nub1 = nub2;
	
// 	//std::cout << "positive bounds: [" << nlb1 << ".." << nub1 << "]" << std::endl;

// 	if(lb_pos<nlb1) {
// 	  lb_pos = nlb1;
// 	  pruning_flag = true;
// 	}
// 	if(ub_pos>nub1) {
// 	  ub_pos = nub1;
// 	  pruning_flag = true;
// 	}
	
// 	if(lb_pos > max_pos[Z] || ub_pos < min_pos[Z]) ppos = true;

//       } // else if(pzero || !zero[Z]) // if(lb_pos || ub_pos)
//       // 	{
//       // 	  lb_pos = min_neg[Z];
//       // 	  ub_pos = max_neg[Z];
//       // 	} else {
//       // 	lb_pos = ub_pos = 0; 
//       // }

//      if(min_neg[Z]<0) {
//        // revise the negative part of Z's domain (if it has one)
//        nlb1 = nlb2 = 0; //lb_pos;
//        nub1 = nub2 = -INFTY; //ub_pos;
	
// 	// it can either be the negitive part of X and the positive part of Y:
// 	if(min_neg[X]<0 && max_pos[Y]>0) {
// 	  // compute the bounds


// 	  nlb1 = min_neg[X]/min_pos[Y];
// 	  nub1 = max_neg[X]/max_pos[Y];
// 	}
// 	// or the negitive part of Y and the positive part of X:
// 	if(max_pos[X]>0 && min_neg[Y]<0) {
// 	  // compute the bounds
// 	  nlb2 = max_pos[X]/max_neg[Y];
// 	  nub2 = min_pos[X]/min_neg[Y];
// 	}

// 	if(nlb1>nlb2) nlb1 = nlb2;
// 	if(nub1<nub2) nub1 = nub2;

// 	//std::cout << "negative bounds: [" << nlb1 << ".." << nub1 << "]" << std::endl;
	
// 	if(lb_neg<nlb1) {
// 	  lb_neg = nlb1;
// 	  pruning_flag = true;
// 	}
// 	if(ub_neg>nub1) {
// 	  ub_neg = nub1;
// 	  pruning_flag = true;
// 	}

// 	if(lb_neg > max_neg[Z] || ub_neg < min_neg[Z]) pneg = true;

//      } // else if(pzero || !zero[Z])// if(lb_neg || ub_neg)
//      //   {
//      // 	 lb_neg = min_pos[Z];
//      // 	 ub_neg = max_pos[Z];
//      //   } else {
//      //   lb_neg = ub_neg = 0;
//      // }
//     }
//   }
  
//   //std::cout << pneg <<  (pzero || !zero[Z]) <<  ppos << std::endl;

//   if(pneg && (pzero || !zero[Z]) && ppos) {
//     wiped = FAILURE(Z);
//   } else if(pruning_flag) {
    
// #ifdef _DEBUG_DIV
//     std::cout << "set bounds to " // << scope[Z].get_domain() << " = " 
// 	      // << scope[X].get_domain() << "/" << scope[Y].get_domain() 
// 	      << "[" << lb_neg << ".." << ub_neg << "|" 
// 	      << (!pzero&&zero[Z] ? "{0}" : "_") << "|" 
// 	      << lb_pos << ".." << ub_pos << "]" << std::endl; 
// #endif
    
//     wiped = prune(lb_neg, ub_neg, lb_pos, ub_pos, pzero, Z);
//   }

//   return wiped;
// }


// Mistral::PropagationOutcome Mistral::PredicateDiv::revise_multiplication(const int X, const int Y, const int Z) {
//   // revise the domain of Z = X*Y 
//   Mistral::PropagationOutcome wiped = CONSISTENT;


// #ifdef _DEBUG_DIV
//   std::cout << "revise bounds of " << scope[Z].get_domain() << " = " 
// 	    << scope[X].get_domain() << "*" << scope[Y].get_domain() 
// 	    // << " = [" << min_neg[Z] << ".." << max_neg[Z] << "|" 
// 	    // << (zero[Z] ? "{0}" : "_") << "|" 
// 	    // << min_pos[Z] << ".." << max_pos[Z] << "]" 
// 	    << std::endl; 
// #endif

  
//   //int lb_pos=1, ub_pos=+INFTY, lb_neg=-INFTY, ub_neg=-1, lb_aux, ub_aux;
//   int lb_pos=min_pos[Z], ub_pos=max_pos[Z], lb_neg=min_neg[Z], ub_neg=max_neg[Z], 
//     nlb1, nlb2, nub1, nub2;
  
//   bool pruning_flag = false, pzero = false;
  

//   // std::cout << min_neg[X] << " " << max_pos[X] << " || " 
//   // 	    <<  min_neg[Y]  << " " << max_pos[Y] << std::endl;

//   // if X = 0 or Y = 0, then Z = 0
//   if( zero[Z] &&
//       !zero[X] && !zero[Y]) {
//     //zero[Z] = 0;
//     pzero = true;
//     pruning_flag = true;
//   }
//   else if((!min_neg[X] && !max_pos[X]) || (!min_neg[Y] && !max_pos[Y])) { 
//     lb_neg = 0;
//     ub_pos = 0;
//     pruning_flag = true;
//   }

//   // std::cout << lb_neg << "\\" << ub_pos << std::endl;

//   if(lb_neg != ub_pos) { 
//     if(IS_OK(wiped)) {
//       if(max_pos[Z]>0) {
// 	// revise the positive part of Z's domain (if it has one)
// 	//ub_pos = 0;
// 	//lb_pos = 1;
// 	//lb_aux = 1;
// 	nlb1 = nlb2 = INFTY; //lb_neg;
// 	nub1 = nub2 = 0; //ub_neg;

// 	// it can either be the positive parts of X and Y:
// 	if(max_pos[X]>0 && max_pos[Y]>0) {
// 	  // compute the bounds
// 	  // ub_pos = max_pos[X] * max_pos[Y];
// 	  // lb_pos = min_pos[X] * min_pos[Y];
// 	  nub1 = max_pos[X] * max_pos[Y];
// 	  nlb1 = min_pos[X] * min_pos[Y];
// 	}
// 	// or the negative parts of X and Y:
// 	if(min_neg[X]<0 && min_neg[Y]<0) {
// 	  // compute the bounds
// 	  // ub_aux = min_neg[X] * min_neg[Y];
// 	  // lb_aux = max_neg[X] * max_neg[Y];
// 	  nub2 = min_neg[X] * min_neg[Y];
// 	  nlb2 = max_neg[X] * max_neg[Y];
// 	}
// 	// if(lb_pos>lb_aux) lb_pos = lb_aux;
// 	// if(ub_pos<ub_aux) ub_pos = ub_aux;
// 	if(nlb1>nlb2) nlb1 = nlb2;
// 	if(nub1<nub2) nub1 = nub2;
	
// 	//std::cout << "positive bounds: [" << nlb1 << ".." << nub1 << "]" << std::endl;

// 	if(lb_pos<nlb1) {
// 	  lb_pos = nlb1;
// 	  pruning_flag = true;
// 	}
// 	if(ub_pos>nub1) {
// 	  ub_pos = nub1;
// 	  pruning_flag = true;
// 	}
//       } else if(pzero || !zero[Z]) {
// 	lb_pos = min_neg[Z];
// 	ub_pos = max_neg[Z];
//       } else {
// 	lb_pos = ub_pos = 0;
//       }

//      if(min_neg[Z]<0) {
//        // revise the negative part of Z's domain (if it has one)
//        //ub_neg = -1;
//        //lb_neg = 0;
//        //ub_aux = -1;
//        nlb1 = nlb2 = 0; //lb_pos;
//        nub1 = nub2 = -INFTY; //ub_pos;
	
// 	// it can either be the negitive part of X and the positive part of Y:
// 	if(min_neg[X]<0 && max_pos[Y]>0) {
// 	  // compute the bounds
// 	  // ub_neg = max_neg[X] * min_pos[Y];
// 	  // lb_neg = min_neg[X] * max_pos[Y];
// 	  nub1 = max_neg[X] * min_pos[Y];
// 	  nlb1 = min_neg[X] * max_pos[Y];

// 	  // std::cout << "\t  1 ub = " << max_neg[X] << "*" << min_pos[Y] << std::endl;
// 	  // std::cout << "\t  1 lb = " << min_neg[X] << "*" << max_pos[Y] << std::endl;

// 	}
// 	// or the negitive part of Y and the positive part of X:
// 	if(max_pos[X]>0 && min_neg[Y]<0) {
// 	  // compute the bounds
// 	  // ub_aux = max_neg[Y] * min_pos[X];
// 	  // lb_aux = min_neg[Y] * max_pos[X];
// 	  nub2 = max_neg[Y] * min_pos[X];
// 	  nlb2 = min_neg[Y] * max_pos[X];

// 	  // std::cout << "\t  2 ub = " << max_neg[Y] << "*" << min_pos[X] << std::endl;
// 	  // std::cout << "\t  2 lb = " << min_neg[Y] << "*" << max_pos[X] << std::endl;
// 	}
// 	// if(lb_neg>lb_aux) lb_neg = lb_aux;
// 	// if(ub_neg<ub_aux) ub_neg = ub_aux;

// 	if(nlb1>nlb2) nlb1 = nlb2;
// 	if(nub1<nub2) nub1 = nub2;


// 	  // std::cout << "change:" << std::endl;
// 	  // std::cout << "\t lbn = " << nlb1 << std::endl;
// 	  // std::cout << "\t ubn = " << nub1 << std::endl;


// 	//std::cout << "negative bounds: [" << nlb1 << ".." << nub1 << "]" << std::endl;
	
// 	if(lb_neg<nlb1) {
// 	  lb_neg = nlb1;
// 	  pruning_flag = true;
// 	}
// 	if(ub_neg>nub1) {
// 	  ub_neg = nub1;
// 	  pruning_flag = true;
// 	}
//      }  else if(pzero || !zero[Z]) {
//        lb_neg = min_pos[Z];
//        ub_neg = max_pos[Z];
//      } else {
//        lb_neg = ub_neg = 0;
//      }
//     }
//   }

//   if(pruning_flag) {
// #ifdef _DEBUG_DIV
//   std::cout << "set bounds to " // << scope[Z].get_domain() << " = " 
// 	    // << scope[X].get_domain() << "*" << scope[Y].get_domain() 
// 	    << "[" << lb_neg << ".." << ub_neg << "|" 
// 	    << (zero[Z] ? "{0}" : "_") << "|" 
// 	    << lb_pos << ".." << ub_pos << "]" << std::endl; 
// #endif
  
//     wiped = prune(lb_neg, ub_neg, lb_pos, ub_pos, pzero, Z);
//   }
  
//   return wiped;
// }


// Mistral::PropagationOutcome Mistral::PredicateDiv::prune(const int lb_neg, 
// 							 const int ub_neg, 
// 							 const int lb_pos, 
// 							 const int ub_pos,
// 							 const bool pzero,
// 							 const int Z) {


// // std::cout << changes << std::endl;
//   Event evt;
//   Mistral::PropagationOutcome wiped = CONSISTENT;
  
//   // int lb = lb_neg;
//   // if(lb>=0) lb = lb_pos;
//   // int ub = ub_pos;
//   // if(ub>=0) lb = lb_pos;

//   if(ub_pos < lb_neg) wiped = FAILURE(Z);
//   else {
//     if(lb_neg>min_neg[Z]) {

//       // std::cout  << lb_neg << ">" << min_neg[Z] 
//       // 		 << " -> update lb" << std::endl;

//       evt = scope[Z].set_min( lb_neg );
//       if( IS_FAIL(evt) ) wiped = FAILURE(Z);
//       else {
// 	if(changes.contain(Z)) {
// 	  event_type[Z] |= evt;
// 	} else {
// 	  event_type[Z] = evt;
// 	  changes.add(Z);
// 	}
// 	min_neg[Z] = scope[Z].get_min();
//       }
//     }
//     if(IS_OK(wiped) && ub_pos<max_pos[Z]) {
      
//       // std::cout  << ub_pos << ">" << max_pos[Z] 
//       // 		 << " -> update ub" << std::endl;

//       evt = scope[Z].set_max( ub_pos );
//       if( IS_FAIL(evt) ) wiped = FAILURE(Z);
//       else {
// 	if(changes.contain(Z)) {
// 	  event_type[Z] |= evt;
// 	} else {
// 	  event_type[Z] = evt;
// 	  changes.add(Z);
// 	}
// 	max_pos[Z] = scope[Z].get_max();
//       }
//     }
//     if(IS_OK(wiped) && (lb_pos>=min_neg[Z] || ub_neg<=max_pos[Z])) { 
      
//       // std::cout << lb_pos << ">=" << min_neg[Z] 
//       // 		<< " or " << ub_neg << "<=" << max_pos[Z]
//       // 		<< " -> may update inbounds" << std::endl;
      
//       if(lb_pos-1>ub_neg && (pzero || (!zero[Z] && (min_pos[Z]<lb_pos || max_neg[Z]>ub_neg)))) {
	  
// 	// std::cout  << lb_pos << ">" << min_pos[Z] 
// 	//  	   << " or " << ub_neg << "<" << max_neg[Z]
// 	// 	   << " -> update inbounds" << std::endl;
	
// 	evt = scope[Z].remove_interval(ub_neg+1, lb_pos-1);
// 	if( IS_FAIL(evt) ) wiped = FAILURE(Z);
// 	else {
// 	  if(changes.contain(Z)) {
// 	    event_type[Z] |= evt;
// 	  } else {
// 	    event_type[Z] = evt;
// 	    changes.add(Z);
// 	  }
// 	  zero[Z] = 0;
// 	  min_pos[Z] = scope[Z].get_min_pos();
// 	  max_neg[Z] = scope[Z].get_max_neg();
// 	}
//       } else {
// 	if(lb_pos>1 && min_pos[Z]<lb_pos) {


// 	  // std::cout << lb_pos << ">" << min_pos[Z] 
// 	  // 	    << " -> update negative ub" << std::endl;

// 	  evt = scope[Z].remove_interval(1, lb_pos-1);
// 	  if( IS_FAIL(evt) ) wiped = FAILURE(Z);
// 	  else {
// 	    //min_pos[Z] = lb_pos;
// 	    //max_neg[Z] = ub_neg;
// 	    if(changes.contain(Z)) {
// 	      event_type[Z] |= evt;
// 	    } else {
// 	      event_type[Z] = evt;
// 	      changes.add(Z);
// 	    }
// 	    min_pos[Z] = scope[Z].get_min_pos();
// 	  }
// 	}
// 	if(ub_neg<-1 && max_neg[Z]>ub_neg) {

// 	  // std::cout 
// 	  //   << ub_neg << "<" << max_neg[Z]
// 	  //   << " -> update positive lb" << std::endl;

// 	  evt = scope[Z].remove_interval(ub_neg+1, -1);
// 	  if( IS_FAIL(evt) ) wiped = FAILURE(Z);
// 	  else {
// 	    //min_pos[Z] = lb_pos;
// 	    //max_neg[Z] = ub_neg;
// 	    if(changes.contain(Z)) {
// 	      event_type[Z] |= evt;
// 	    } else {
// 	      event_type[Z] = evt;
// 	      changes.add(Z);
// 	    }
// 	    max_neg[Z] = scope[Z].get_max_neg();
// 	  }
// 	}
//       }
//     }
//   }

//   if((min_neg[Z]>0 && min_neg[Z]<min_pos[Z]) ||
//      (!min_neg[Z] && !zero[Z]))
//     min_neg[Z] = min_pos[Z];

//   if((max_pos[Z]<0 && max_pos[Z]>max_neg[Z]) ||
//      (!max_pos[Z] && !zero[Z]))
//     max_pos[Z] = max_neg[Z];




//   //std::cout << changes << std::endl;
// #ifdef _DEBUG_DIV
// 	if(IS_OK(wiped)) {
// 	  std::cout << scope[0].get_domain() 
// 		    << " * " << scope[1].get_domain() 
// 		    << " = " << scope[2].get_domain() << std::endl;
// 	} else std::cout << "FAIL!" << std::endl ;

// 	std::cout
// 	  << " now in [" << min_neg[Z] << ".." << max_neg[Z] << "|" 
// 	  << (zero[Z] ? "{0}" : "_") << "|" 
// 	  << min_pos[Z] << ".." << max_pos[Z] << "]" << std::endl;

//   std::cout << std::endl; 
// #endif


//   return wiped;
// }





// Mistral::PropagationOutcome Mistral::PredicateDiv::propagate() {      
//   Mistral::PropagationOutcome wiped = CONSISTENT;
  

// #ifdef _DEBUG_DIV
//   std::cout << std::endl << std::endl << "propagate " // << this 
// 	    << std::endl;
// #endif

//   for(int i=0; i<3; ++i) {
//     max_pos[i] = scope[i].get_max();
//     min_neg[i] = scope[i].get_min();

//     zero[i] = scope[i].contain(0);

//     if(min_neg[i]<0)
//       max_neg[i] = scope[i].get_max_neg();
//     else
//       max_neg[i] = min_neg[i];

//     if(max_pos[i]>0)
//       min_pos[i] = scope[i].get_min_pos();
//     else
//       min_pos[i] = max_pos[i];

//   }

 
  
// #ifdef _DEBUG_DIV
//   std::cout << scope[0]/*.get_var()*/ << " in " << scope[0].get_domain() 
// 	    << " / " << scope[1]/*.get_var()*/ << " in " << scope[1].get_domain() 
//     	    << " = " << scope[2]/*.get_var()*/ << " in " << scope[2].get_domain() << std::endl;
// #endif

//   //int i, j, lb, ub, evt_idx, rev_idx, aux_idx;
//   int evt_idx;
//   //Event evt;

//   if(zero[1] && scope[1].remove(0) == FAIL_EVENT) {
//     wiped = FAILURE(1);
//   }


//   while(IS_OK(wiped) && !changes.empty()) {

//     evt_idx = changes.pop();

// #ifdef _DEBUG_DIV
//     std::cout << "react to " << scope[evt_idx]/*.get_var()*/ << " in " 
// 	      << scope[evt_idx].get_domain() 
// 	      << (LB_CHANGED(event_type[evt_idx]) ? " (change on LB) " : "")
// 	      << (UB_CHANGED(event_type[evt_idx]) ? " (change on UB) " : "")
// 	      << std::endl;
// #endif

//     // x0 / x1 = x2 
//     if(evt_idx < 2) {
//       // we update x2 = x0/x1
//       wiped = revise_integer_division(0, 1, 2);
//       // revise x(1-evt)
//       if(IS_OK(wiped)) {
// 	if(evt_idx) // revise x0
// 	  wiped = revise_multiplication(1, 2, 0);
// 	else // revise x1 (x2 = x0/x1) -> x1 = x0/x2
// 	  wiped = revise_integer_division(0, 2, 1);
//       }
//     } else {
//       // update x0 = x1*x2 
//       wiped = revise_multiplication(1, 2, 0);
//       // update x1 = x0/x2
//       if(IS_OK(wiped)) wiped = revise_integer_division(0, 2, 1);
//     }
//   }
    
//   return wiped;
// }
  
// std::ostream& Mistral::PredicateDiv::display(std::ostream& os) const {
//   os << scope[2] << " == (" << scope[0] << " / " << scope[1] << ")";
//   return os;
// }



Mistral::ConstraintGAC4::ConstraintGAC4(Vector< Variable >& scp)
  : ConstraintTable(scp) { }

Mistral::ConstraintGAC4::ConstraintGAC4(std::vector< Variable >& scp)
  : ConstraintTable(scp) { }

void Mistral::ConstraintGAC4::initialise() {
  ConstraintTable::initialise();

  unsigned int arity = scope.size;
  int n, m;

  // for(unsigned int i=0; i<arity; ++i)
  //   trigger_on(_DOMAIN_, scope[i]);

  

  values = new Vector<int>[arity];

#ifdef _OS_PRUNING
  list_map = new int*[arity];
  support_of = new ReversibleSet**[arity];
#else
  support_of = new ReversibleSet*[arity];
#endif


  n = 0;
  for(unsigned int i=0; i<arity; ++i) {
    n += scope[i].get_size();
  }

#ifdef _OS_PRUNING
  support_lists = new ReversibleSet[n];
  var_map = new int[n];
  val_map = new int[n];
  pruned_lists.initialise(0, n-1, n, false);
  list_pruning = new Vector<int>[n];
#endif

#ifdef _DEBUG_TABLE
  init_support_size = new int[n];
#endif

  n = 0; 
  for(unsigned int i=0; i<arity; ++i) {
    
    m = scope[i].get_initial_max() - scope[i].get_initial_min() + 1;

#ifdef _OS_PRUNING
    list_map[i] = new int[m];
    support_of[i] = new ReversibleSet*[m];
    std::fill(list_map[i], list_map[i]+m, -1);    
    list_map[i] -= scope[i].get_initial_min();
#else
    support_of[i] = new ReversibleSet[m];
#endif

    support_of[i] -= scope[i].get_initial_min();
    values[i].initialise(0,scope[i].get_size());

    int vnxt = scope[i].get_first(), val;
    do {
      val = vnxt;

#ifdef _OS_PRUNING
      if(values[i].empty()) {
  	support_lists[n].initialise(solver, 
				      0, table.size, 8, false);
      } else {
	support_lists[n].initialise(*(support_of[i][values[i][0]]), 8);
      }
      var_map[n] = i;
      val_map[n] = val;
      list_map[i][val] = n;
      support_of[i][val] = &(support_lists[n]);
#else
      if(values[i].empty()) {
  	support_of[i][val].initialise(solver, 
				      0, table.size, 8, false);
      } else {
	support_of[i][val].initialise(support_of[i][values[i][0]], 8);
      }
#endif
      
      values[i].add(val);
      
      ++n;
      vnxt = scope[i].next(val);
    } while( val != vnxt );
  }
  

  for(unsigned int k=0; k<table.size; ++k) {
    //std::cout << "add " << table[k] << std::endl;
    for(unsigned int i=0; i<arity; ++i) {
#ifdef _OS_PRUNING
      support_of[i][table[k][i]]->init_add(k);
#else
      support_of[i][table[k][i]].init_add(k);
#endif
    }
  }

#ifdef _DEBUG_TABLE
  for(int i=0; i<n; ++i) {
    init_support_size[i] = support_lists[i].size;
  }
#endif

  //exit(1);


  GlobalConstraint::initialise();

  value_delta = new DomainDelta[arity];
  // initialise the domain_delta
  for(unsigned int i=0; i<arity; ++i) {
    value_delta[i].initialise(scope[i]);
  }


  // then we "propagate"
  for(unsigned int xi=0; xi<arity; ++xi) {
    Domain dom_xi(scope[xi]);
    //std::cout << "iterate over " << scope[xi].get_domain() << std::endl;
     
    Domain::iterator xstop = dom_xi.begin();

    int valj;
    for(Domain::iterator xit = dom_xi.end(); xit != xstop; ) {
      valj = dom_xi.get_value(--xit); 

#ifdef _OS_PRUNING
      if(support_of[xi][valj]->empty())  {
	if(FAILED(scope[xi].remove(valj))) get_solver()->fail();
      }
#else
      if(support_of[xi][valj].empty())  {
	if(FAILED(scope[xi].remove(valj))) get_solver()->fail();
      }
#endif

    }
  }

  //if(!check_integrity()) exit(1);
}

Mistral::ConstraintGAC4::~ConstraintGAC4() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete gac4 constraint" << std::endl;
#endif

  for(unsigned int i=0; i<scope.size; ++i) {
    for(unsigned int j=1; j<values[i].size; ++j) {
      // support_of[i][values[i][j]]->index_ = NULL;
      // support_of[i][values[i][j]].start_ = NULL;

#ifdef _OS_PRUNING
      support_of[i][values[i][j]]->neutralise();
#else
      support_of[i][values[i][j]].neutralise();
#endif
    }
    support_of[i] += scope[i].get_initial_min();
    delete [] support_of[i];

#ifdef _OS_PRUNING
    list_map[i] += scope[i].get_initial_min();
    delete [] list_map[i];
#endif 

    //delete value_delta[i].cleanup();
  }

  delete [] support_of;

#ifdef _OS_PRUNING
  delete [] var_map;
  delete [] val_map;
  delete [] list_map;
  delete [] support_lists;
  delete [] list_pruning;
#endif

  delete [] value_delta;
  delete [] values;

}



Mistral::PropagationOutcome Mistral::ConstraintGAC4::propagate() 
{
  PropagationOutcome wiped = CONSISTENT;
 
  int xi, xj, vali, valj, tk;
  Domain::iterator xstop;
  Domain::iterator xit;

  int arity = scope.size;

#ifdef _OS_PRUNING
  int asgn;
#endif

#ifdef _DEBUG_TABLE
  if(_DEBUG_TABLE) {
  std::cout << "propagate " << id << " " << *this << std::endl;
  //display_supports(std::cout);
  }
#endif

  while(!changes.empty()) {

#ifdef _DEBUG_TABLE
    if(_DEBUG_TABLE) {
    std::cout << "modified variables: " << changes << std::endl;
    }
#endif

    xi = changes.pop();

#ifdef _DEBUG_TABLE
    if(_DEBUG_TABLE) {
    std::cout << "  changes on " << scope[xi] << ":\n";
    }
#endif

    xstop = value_delta[xi].begin();
    for(xit = value_delta[xi].end(); --xit>=xstop; ) {
      vali = *xit;

#ifdef _DEBUG_TABLE
      if(_DEBUG_TABLE) {
	std::cout << "    lost " << vali << ": " << std::endl;
      }
#endif

#ifdef _OS_PRUNING
      ReversibleSet::iterator xit = support_of[xi][vali]->begin();
      ReversibleSet::iterator xend = support_of[xi][vali]->end();
#else
      ReversibleSet::iterator xit = support_of[xi][vali].begin();
      ReversibleSet::iterator xend = support_of[xi][vali].end();
#endif
      while(xit != xend) {
	tk = *xit;

	for(xj = arity; xj--;) {
	  if(xj != xi
	     //&& active.contain(xj)
	     ) {
	    valj = table[tk][xj];

#ifdef _OS_PRUNING
	    asgn = list_map[xj][valj];
	    if(!pruned_lists.contain(asgn)) pruned_lists.add(asgn);
	    list_pruning[asgn].add(tk);
#else
	    support_of[xj][valj].reversible_remove(tk);
	    if(support_of[xj][valj].empty()) {
	      if(FAILED(scope[xj].remove(valj))) { wiped = FAILURE(xj); goto FAIL; }
	    }
#endif
	  }
	}
	++xit;
      }

#ifdef _DEBUG_TABLE
    if(_DEBUG_TABLE) {
      std::cout << " " << pruned_lists << std::endl;
    }
#endif
    
    }
  }

#ifdef _OS_PRUNING
  for(sk=pruned_lists.size; --sk>=0;) {
    tk = pruned_lists[sk];
    
#ifdef _DEBUG_TABLE
    if(_DEBUG_TABLE) {
    std::cout << "support[" << scope[var_map[tk]] << "][" << val_map[tk] << "] = " 
	      << support_lists[tk] << "\n            - " 
	      << list_pruning[tk] << std::endl;
    }
#endif

    support_lists[tk].reversible_remove(list_pruning[tk]);
    if(support_lists[tk].empty()) {

#ifdef _DEBUG_TABLE
      if(_DEBUG_TABLE) {
      std::cout << "support[" << scope[var_map[tk]] << "][" << val_map[tk] << "] is emptied -> pruning!\n";  
      }
#endif

      if(FAILED(scope[var_map[tk]].remove(val_map[tk]))) { wiped = FAILURE(var_map[tk]); goto FAIL; }
    }

    list_pruning[tk].clear();
  }
  pruned_lists.clear();
#endif

  for(xi=0; xi<arity; ++xi) {
    value_delta[xi].close();
  }
  
 FAIL: 

#ifdef _OS_PRUNING
  for(sk=pruned_lists.size; --sk>=0;) {
    tk = pruned_lists[sk];
    list_pruning[tk].clear();
  }
  pruned_lists.clear();
#endif

#ifdef _DEBUG_TABLE
  if(_DEBUG_TABLE) {
    display_supports(std::cout);
  }
#endif

  return wiped;
}

#ifdef _DEBUG_TABLE
bool Mistral::ConstraintGAC4::check_integrity( ) const 
{
  bool ok = true;
  
  int xi, vali, tk, xj;
  int arity = scope.size;

  bool entailed = active.size <= 1;
  

  for(xi=0; xi<arity; ++xi) {
    Domain dom_xi(scope[xi]);
    Domain::iterator xend = dom_xi.end();
    for(Domain::iterator xit = dom_xi.begin(); xit != xend; ++xit) {
      
      vali = dom_xi.get_value(xit);
      ReversibleSet::iterator first = support_of[xi][vali]->begin();
      ReversibleSet::iterator last = support_of[xi][vali]->end();
      ReversibleSet::iterator past = first+init_support_size[list_map[xi][vali]];
	//support_of[xi][vali]->end_mem();


      //std::cout << first << " " << last << " " << past << std::endl;
      
      // check that every "current" support is valid
      if(!entailed) for(ReversibleSet::iterator sit = first; sit != last; ++sit) {
	tk = *sit;
	for(xj=0; xj<arity; ++xj) {
	  if(!(scope[xj].contain(table[tk][xj]))) {
	    std::cout << this << " " << id << ": tuple " << tk << ": <" << table[tk][0];
	    for(int xk=1; xk<arity; ++xk) {
	      std::cout << " " << table[tk][xk];
	    }
	    std::cout << "> is not valid! " << scope[0].get_domain();
	    for(int xk=1; xk<arity; ++xk) {
	      std::cout << "x" << scope[xk].get_domain();
	    }
	    std::cout << std::endl;
	    //exit(1);
	    ok = false;
	  }
	}
      }
      
      // check that every "pruned" support is not valid
      for(ReversibleSet::iterator sit = last; sit != past; ++sit) {
	tk = *sit;
	bool valid = true;
	for(xj=0; xj<arity; ++xj) {
	  valid &= scope[xj].contain(table[tk][xj]);
	}
	if(valid) {
	  std::cout << this << " " << id << ": tuple " << tk << ": <" << table[tk][0];
	  for(int xk=1; xk<arity; ++xk) {
	    std::cout << " " << table[tk][xk];
	  }
	  std::cout << "> is valid!\n";
	  //exit(1);
	  ok =false;
	}
      }     
    }
  }
  return ok;
}
#endif

int Mistral::ConstraintGAC4::check( const int* s ) const 
{
  int xmin=0, found = false;
  const int *support;
  unsigned int arity = scope.size, min = INFTY;
  for(unsigned int i=0; i<arity; ++i) {
#ifdef _OS_PRUNING
    if(support_of[i][s[i]]->list_capacity < min) {
      xmin = i;
      min = support_of[i][s[i]]->list_capacity;
    }
#else
    if(support_of[i][s[i]].list_capacity < min) {
      xmin = i;
      min = support_of[i][s[i]].list_capacity;
    }
#endif
  }

#ifdef _OS_PRUNING
  ReversibleSet::iterator end = support_of[xmin][s[xmin]]->end_mem();
  ReversibleSet::iterator it = support_of[xmin][s[xmin]]->begin();
#else
  ReversibleSet::iterator end = support_of[xmin][s[xmin]].end_mem();
  ReversibleSet::iterator it = support_of[xmin][s[xmin]].begin();
#endif

  for(; !found && it!=end; ++it) {
    support = table[*it];
    found = true;
    for(unsigned int i=0; found && i<arity; ++i) {
      if(support[i] != s[i]) found = false;
    }
  }

  return !found;
}

std::ostream& Mistral::ConstraintGAC4::display_supports(std::ostream& os) const {
  unsigned int arity = scope.size;
  for(unsigned int xi=0; xi<arity; ++xi) {
    
    os << scope[xi] << " in " << scope[xi].get_domain() << std::endl;

    Domain dom_xi(scope[xi]);
    
    Domain::iterator xstop = dom_xi.end();
    
    int valj;
    for(Domain::iterator xit = dom_xi.begin(); xit!=xstop; ++xit) {
      valj = dom_xi.get_value(xit);   
      os << "    = " << valj << ":" ;
      
#ifdef _OS_PRUNING
      ReversibleSet::iterator sit = support_of[xi][valj]->begin();
      ReversibleSet::iterator send = support_of[xi][valj]->end();
#else
      ReversibleSet::iterator sit = support_of[xi][valj].begin();
      ReversibleSet::iterator send = support_of[xi][valj].end();
#endif
      
      while(sit != send) {
	os << " <" << table[*sit][0];
	for(unsigned int xj=1; xj<arity; ++xj) 	os << " " << table[*sit][xj];
	os << ">";
	++sit;
      }

      os << std::endl;
	//<< support_of[xi][valj] << std::endl;
    }
    os << std::endl;  
  }
  return os;
}

std::ostream& Mistral::ConstraintGAC4::display(std::ostream& os) const {
  os << "TABLE_GAC4(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << ", " << scope[i]/*.get_var()*/;
  os << ")";
  
  //display_supports(os);

  // os << ") in " ;
  // for(unsigned int i=0; i<table.size; ++i) {
  //   os << "<" << table[i][0] ;
  //   for(unsigned int j=1; j<scope.size; ++j) {
  //     os << " " << table[i][j] ;
  //   }
  //   os << ">";
  // }
  return os;
}



Mistral::ConstraintTable::ConstraintTable(Vector< Variable >& scp)
  : GlobalConstraint(scp) { priority = 0; }

Mistral::ConstraintTable::ConstraintTable(std::vector< Variable >& scp)
  : GlobalConstraint(scp) { priority = 0; }

Mistral::ConstraintTable::~ConstraintTable() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete table constraint" << std::endl;
#endif
}

void Mistral::ConstraintTable::initialise() {
  ConstraintImplementation::initialise();
  unsigned int arity = scope.size;

  for(unsigned int i=0; i<arity; ++i)
    trigger_on(_DOMAIN_, scope[i]);
}

void Mistral::ConstraintTable::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), RANGE_VAR);
}


int Mistral::ConstraintTable::check( const int* s ) const 
{
  bool found = false;
  unsigned int arity = scope.size;
  const int* support;
  Vector<const int*>::iterator end = table.end();
  for(Vector<const int*>::iterator it = table.begin(); !found && it!=end; ++it) {
    support = *it;
    found = true;
    for(unsigned int i=0; found && i<arity; ++i) {
      if(support[i] != s[i]) found = false;
    }
  }

  return !found;
}

std::ostream& Mistral::ConstraintTable::display(std::ostream& os) const {
  os << "TABLE(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << ", " << scope[i]/*.get_var()*/;
  os << ")";
  
  //display_supports(os);

  // os << ") in " ;
  // for(unsigned int i=0; i<table.size; ++i) {
  //   os << "<" << table[i][0] ;
  //   for(unsigned int j=1; j<scope.size; ++j) {
  //     os << " " << table[i][j] ;
  //   }
  //   os << ">";
  // }
  return os;
}

void Mistral::ConstraintTable::add(const int* tuple) {
  table.add(tuple);
}





Mistral::ConstraintGAC2001::ConstraintGAC2001(Vector< Variable >& scp)
  : ConstraintTable(scp) { }

Mistral::ConstraintGAC2001::ConstraintGAC2001(std::vector< Variable >& scp)
  : ConstraintTable(scp) { }

void Mistral::ConstraintGAC2001::initialise() {
  ConstraintTable::initialise();
  unsigned int arity = scope.size;

  // for(unsigned int i=0; i<arity; ++i)
  //   trigger_on(_DOMAIN_, scope[i]);


  unsigned int i, j;
  int nval;

  firstSupport = new ReversibleNum<int>*[arity];
  themins = new int[arity];
  order = new int[arity];
  supportList = new Vector<const int*>*[arity];


  for(i=0; i<arity; ++i) {
    order[i] = i;
    themins[i] = scope[i].get_initial_min();
    nval=(scope[i].get_initial_max() - themins[i] + 1);

    /// init the reversible data structure 'firstSupport'
    firstSupport[i] = new ReversibleNum<int>[nval];
    firstSupport[i] -= themins[i];

    Domain dom_xi(scope[i]);
    Domain::iterator xit = dom_xi.begin();
    Domain::iterator xend = dom_xi.end();

    while(xit != xend) {
      nval = dom_xi.get_value(xit);
      firstSupport[i][nval].initialise(solver, 0);
      ++xit;
    }
  }

  GlobalConstraint::initialise();

  for(i=0; i<arity; ++i) {
    /// init the list of supports for each value
    supportList[i] = new Vector<const int*> [scope[i].get_initial_max() - themins[i] + 1];
    supportList[i] -= themins[i];
  }

  i=table.size;
  while( i-- ) {
    for(j=0; j<arity; ++j) 
      supportList[j][table[i][j]].add( table[i] );
  }

}

Mistral::ConstraintGAC2001::~ConstraintGAC2001() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete table gac 2001 constraint" << std::endl;
#endif

  int i, arity = scope.size;
  for(i=0; i<arity; ++i) {
    firstSupport[i] += themins[i];
    delete [] firstSupport[i];
  }
  delete [] firstSupport;
  delete [] order;

  for(i=0; i<arity; ++i) {
    supportList[i] += themins[i];
    delete [] supportList[i];
  }
  delete [] supportList;
  delete [] themins;

}



Mistral::PropagationOutcome Mistral::ConstraintGAC2001::propagate() 
{
  PropagationOutcome wiped = CONSISTENT;

  int i, oi, j, k, ok, valid, index, n, arity = scope.size, changedIdx;



#ifdef _DEBUG_TABLE
  if(_DEBUG_TABLE) {
  std::cout << "\npropagate " << *this << std::endl;
  std::cout << active << std::endl;
  }
#endif

 
  for(i=1; i<arity; ++i) {   // order variables by increasing domain size
    j=i;
    while( j && scope[order[j]].get_size() < scope[order[j-1]].get_size() ) {      
      ok = order[j];
      order[j] = order[j-1];
      order[--j] = ok;
    }
  }



  while(IS_OK(wiped) && !changes.empty()) {

#ifdef _DEBUG_TABLE
    if(_DEBUG_TABLE) {
    std::cout << "modified variables: " << changes << std::endl;
    }
#endif

    changedIdx = changes.pop();


#ifdef _DEBUG_TABLE
    if(_DEBUG_TABLE) {
    std::cout << "  changes on " << scope[changedIdx] << ":\n";
    }
#endif

    
    for( i=0; IS_OK(wiped) && i<arity; ++i ) {
      oi = order[i];
      if( oi != changedIdx && active.contain(oi) ) 
	{
	  
#ifdef _DEBUG_TABLE
	  if(_DEBUG_TABLE) {
	  std::cout << "    revise " << scope[oi] << " in " << scope[oi].get_domain() << ":\n";
	  }
#endif

	  Domain dom_xi(scope[oi]);
	  
	  Domain::iterator xit = dom_xi.end();
	  Domain::iterator xend = dom_xi.begin();

	  while(xit != xend && IS_OK(wiped)) {
	    j = dom_xi.get_value(--xit);

	    n = supportList[oi][j].size;
	    supports_X = supportList[oi][j].stack_; // init the list of supports

#ifdef _DEBUG_TABLE
	    if(_DEBUG_TABLE) {
	    std::cout << "      = " << j << ": " ;
	    }
#endif
	    
	    valid = false;
	    for(index=firstSupport[oi][j]; !valid && index<n; ++index) {


#ifdef _DEBUG_TABLE
	      if(_DEBUG_TABLE) {
	    std::cout << "<" << supports_X[index][0];
	    for(k=1; k<arity; ++k)
	      std::cout << " " << supports_X[index][k];
	    std::cout << ">" ;
	      }
#endif

	      valid = true;
	      for(k=0; valid && k<arity; ++k) {
		ok = order[k];
		valid = ( oi == ok || scope[ok].contain( supports_X[index][ok] ) );
	      }
	    }
	    if( valid ) firstSupport[oi][j] = index-1;
	    else {
// #ifdef _DEBUG_TABLE
// 	      if(_DEBUG_TABLE) {
// 		std::cout << "remove " << j << " from " << scope[oi].get_domain() << std::endl;
	      
		if(FAILED(scope[oi].remove( j ))) wiped = FAILURE(oi);
		
		//std::cout << wiped << std::endl;
	    }

#ifdef _DEBUG_TABLE
	    if(_DEBUG_TABLE) {
	    if( valid ) {
	      std::cout << " ok!";
	    } else {
	      std::cout << " empty!" ;
	    }
	    std::cout << std::endl;
	    }
#endif
	    
	    //--xit;
	  }

#ifdef _DEBUG_TABLE
	    if(_DEBUG_TABLE) {
	      std::cout << "    revised " << scope[oi] << " in " << scope[oi].get_domain() << ":\n\n";
	    }
#endif
	}
    }
  }

#ifdef _DEBUG_TABLE
  if(_DEBUG_TABLE) {
    std::cout << "return " << (IS_OK(wiped) ? "consistent\n" : "failure\n");
  }
#endif

  return wiped;
}

int Mistral::ConstraintGAC2001::check( const int* s ) const 
{



  int xmin=0, found = false, t;
  const int *support;
  unsigned int arity = scope.size, min = INFTY;

  // std::cout << "check " << *this << " (" << s[0];
  // for(unsigned int i=1; i<arity; ++i) {
  //   std::cout << " " << s[i];
  // }
  // std::cout << ")" << std::endl;


  for(unsigned int i=0; i<arity; ++i) {
    if(supportList[i][s[i]].size < min) {
      xmin = i;
      min = supportList[i][s[i]].size;
    }
  }


  t = supportList[xmin][s[xmin]].size;
  while(!found && t--) {
    support = supportList[xmin][s[xmin]][t];
    found = true;
    //std::cout << "<"
    for(unsigned int i=0; found && i<arity; ++i) {
      //std::cout << " " << support[i];
      if(support[i] != s[i]) found = false;
    }
    // if(found)
    //   std::cout << "> OK!" << std::endl;
  }
  
  return !found;
}

std::ostream& Mistral::ConstraintGAC2001::display_supports(std::ostream& os) const {
  unsigned int arity = scope.size;
  for(unsigned int xi=0; xi<arity; ++xi) {
    
    os << scope[xi] << " in " << scope[xi].get_domain() << std::endl;

    Domain dom_xi(scope[xi]);
    
    Domain::iterator xstop = dom_xi.end();
    
    int valj;
    for(Domain::iterator xit = dom_xi.begin(); xit!=xstop; ++xit) {
      valj = dom_xi.get_value(xit);   
      os << "    = " << valj << ":" ;
      Vector<const int*>::iterator send = supportList[xi][valj].end();
      for(Vector<const int*>::iterator sit = supportList[xi][valj].begin(); sit != send; ++sit) {
	os << " <" << (*sit)[0] ;
	for(unsigned int xj=1; xj<arity; ++xj)
	  os << " " << (*sit)[xj] ;
	os << ">";
      }
      os << std::endl;
      //<< support_of[xi][valj] << std::endl;
    }
    os << std::endl;  
  }
  return os;
}

std::ostream& Mistral::ConstraintGAC2001::display(std::ostream& os) const {
  os << "TABLE_GAC2001(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << ", " << scope[i]/*.get_var()*/;
  os << ")";
  
  display_supports(os);

  // os << ") in " ;
  // for(unsigned int i=0; i<table.size; ++i) {
  //   os << "<" << table[i][0] ;
  //   for(unsigned int j=1; j<scope.size; ++j) {
  //     os << " " << table[i][j] ;
  //   }
  //   os << ">";
  // }
  return os;
}

// void Mistral::ConstraintTableGAC2001Allowed::add(const int* tuple) {
//   tuples.add(tuple);
// }




int Mistral::ConstraintGAC3::getpos(const int *sol) const 
{
  int i = scope.size-1;
  int pos = sol[i]; 
  while( i-- ) pos += (sol[i])*var_sizes[i]; 
  return pos;
}

//ConstraintGAC3::ConstraintGAC3() : ConstraintTable() {}

Mistral::ConstraintGAC3::ConstraintGAC3(Vector< Variable >& scp) : ConstraintTable(scp) {}

Mistral::ConstraintGAC3::ConstraintGAC3(std::vector< Variable >& scp) : ConstraintTable(scp) {}

bool Mistral::ConstraintGAC3::isValid(const int *tuple) const
{
  int i=scope.size, valid=true;
  while(valid && i--)
    valid = scope[i].contain(tuple[i]);
  return valid;
}

void Mistral::ConstraintGAC3::initialise() {
  ConstraintTable::initialise();
  int arity = scope.size;

  int i;
  int matrixmin = scope[arity-1].get_initial_min();
  int matrixmax = scope[arity-1].get_initial_max();
    
  var_sizes = new int[arity-1];
  var_sizes[arity-2] = (scope[arity-1].get_initial_max() - scope[arity-1].get_initial_min() + 1);
  for(i=arity-3; i >= 0; --i) {
    var_sizes[i] = var_sizes[i+1] * 
      (scope[i+1].get_initial_max() - scope[i+1].get_initial_min() + 1);
  }
    
  i = arity-1;
  while( i-- ) {
    matrixmax += (scope[i].get_initial_max())*var_sizes[i]; 
    matrixmin += (scope[i].get_initial_min())*var_sizes[i]; 
  }
    
  matrix.initialise(matrixmin, matrixmax, BitSet::empt);
    
  //if( spin ) {
    matrix.fill();
    i=table.size;
    while( i-- ) {
      if(isValid(table[i]))
	matrix.remove(getpos(table[i]));
    }
    
  // } else {
  //   matrix.clear();
  //   i=tuples.size;
  //   while( i-- )
  //     if(isValid(tuples[i]))
  // 	matrix.insert(getpos(tuples[i]));
  // }
  
  GlobalConstraint::initialise();

}

Mistral::ConstraintGAC3::~ConstraintGAC3() 
{ 
  delete [] var_sizes;
}

int Mistral::ConstraintGAC3::check( const int* s ) const 
{
  return matrix.contain(getpos(s));
}

// void Mistral::ConstraintGAC3::fillNogood() 
// {
//   matrix.fill();
// }

// void Mistral::ConstraintGAC3::fillSupport() 
// {
//   matrix.clear();
// }

// void Mistral::ConstraintGAC3::addNogood(const int* sol) 
// {
//   matrix.insert(getpos(sol));
// }

// void Mistral::ConstraintGAC3::addSupport(const int* sol) 
// {
//   matrix.erase(getpos(sol));
// }


std::ostream& Mistral::ConstraintGAC3::display_supports(std::ostream& os) const {
  // unsigned int arity = scope.size;
  // for(unsigned int xi=0; xi<arity; ++xi) {
    
  //   os << scope[xi] << " in " << scope[xi].get_domain() << std::endl;

  //   Domain dom_xi(scope[xi]);
    
  //   Domain::iterator xstop = dom_xi.end();
    
  //   int valj;
  //   for(Domain::iterator xit = dom_xi.begin(); xit<xstop; ++xit) {
  //     valj = dom_xi.get_value(xit);   
  //     os << "    = " << valj << ":" ;
  //     Vector<const int*>::iterator send = supportList[xi][valj].end();
  //     for(Vector<const int*>::iterator sit = supportList[xi][valj].begin(); sit != send; ++sit) {
  // 	os << " <" << (*sit)[0] ;
  // 	for(unsigned int xj=1; xj<arity; ++xj)
  // 	  os << " " << (*sit)[xj] ;
  // 	os << ">";
  //     }
  //     os << std::endl;
  //     //<< support_of[xi][valj] << std::endl;
  //   }
  //   os << std::endl;  
  // }
  return os;
}

std::ostream& Mistral::ConstraintGAC3::display(std::ostream& os) const {
  os << "TABLE_GAC3(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << ", " << scope[i]/*.get_var()*/;
  os << ")";
  
  //display_supports(os);

  // os << ") in " ;
  // for(unsigned int i=0; i<table.size; ++i) {
  //   os << "<" << table[i][0] ;
  //   for(unsigned int j=1; j<scope.size; ++j) {
  //     os << " " << table[i][j] ;
  //   }
  //   os << ">";
  // }
  return os;
}


//#define _DEBUG_CARD true

Mistral::ConstraintBoolSumEqual::ConstraintBoolSumEqual(Vector< Variable >& scp, const int t)
  : GlobalConstraint(scp) { 
  priority = 1;
  total = t; 
  lower_bound = t;
  upper_bound = t;
}

void Mistral::ConstraintBoolSumEqual::initialise() {
  ConstraintImplementation::initialise();
  int _min_ = 0;
  int _max_ = scope.size;
  for(unsigned int i=0; i<scope.size; ++i) {
    trigger_on(_VALUE_, scope[i]);
    if(scope[i].is_ground()) {
      if(scope[i].get_value()) ++_min_;
      else --_max_;
    }
  }

  min_.initialise(get_solver(), _min_);
  max_.initialise(get_solver(), _max_);

  GlobalConstraint::initialise();
}

void Mistral::ConstraintBoolSumEqual::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}

Mistral::ConstraintBoolSumEqual::~ConstraintBoolSumEqual() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete boolsumequal constraint" << std::endl;
#endif
}

Mistral::PropagationOutcome Mistral::ConstraintBoolSumEqual::propagate() 
{
  PropagationOutcome wiped_idx = CONSISTENT;

#ifdef _DEBUG_CARD
  if(_DEBUG_CARD) {
    std::cout << "c" << id << ":\n";
    std::cout << "changes: " << changes << std::endl;
    std::cout << " events: " << events << std::endl;
    
    int new_evt[scope.size];
    for(int i=0; i<scope.size; ++i) {
      new_evt[i]=0;
    }
    for(int i=0; i<events.size; ++i) {
      new_evt[events[i]] = 1;
    }
    std::cout << " propagate [" ;
    for(int i=0; i<scope.size; ++i) {
      std::cout << std::setw(5) << scope[i].get_domain();
    }
    std::cout << "]\n   changes [" ;
    for(int i=0; i<scope.size; ++i) {
      std::cout << std::setw(5) << (new_evt[i] ? scope[i].get_domain() : " ");
    }
    std::cout << "]\n was in [" << min_ << ".." << max_ << "] now in [";
  }
#endif



  while(!events.empty()) {
    if(LB_CHANGED(event_type[events.pop()])) ++min_;
    else --max_;
  }

#ifdef _DEBUG_CARD
  if(_DEBUG_CARD) {
  std::cout << min_ << ".." << max_ << "]\n";
 }
#endif

  // if(min_>upper_bound || max_<lower_bound) {
  //   wiped_idx = FAILURE(events[0]);
  // } else if(min_==upper_bound) {
  //   // here there should not be any failure
  //   for(unsigned int i=0; i<active.size; ++i) {
  //     scope[active[i]].set_domain(0);
  //   }
  // } else if(max_==lower_bound) {
  //   // here there should not be any failure
  //   for(unsigned int i=0; i<active.size; ++i) {
  //     scope[active[i]].set_domain(1);
  //   }
  // }

  if(min_>upper_bound || max_<lower_bound) {
    
#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "FAILURE!\n";
    }
#endif

    wiped_idx = FAILURE(events[0]);
  } else if(min_==upper_bound) {

#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "UPPER BOUND REACHED!\n";
    }
#endif

    // here there should not be any failure
    for(unsigned int i=0; i<active.size; ++i) {
      if(!(scope[active[i]].is_ground()) && FAILED(scope[active[i]].set_domain(0))) {
	wiped_idx = FAILURE(i);
	break;
      }
    }
  } else if(max_==lower_bound) {

#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "LOWER BOUND REACHED!\n";
    }
#endif

    // here there should not be any failure
    for(unsigned int i=0; i<active.size; ++i) {
      if(!(scope[active[i]].is_ground()) && FAILED(scope[active[i]].set_domain(1))) {
	wiped_idx = FAILURE(i);
	break;
      }
    }
  } else if(min_ >= lower_bound && max_ <= upper_bound) {

#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "ENTAILED!\n";
    }
#endif

    relax();
  }

#ifdef _DEBUG_CARD
  else {
    if(_DEBUG_CARD) {
      std::cout << "OK\n";
    }
  }
#endif




  // int _min_ = 0;
  // int _max_ = 0;
  // unsigned int i;
  // gap = 0;

  // for( i=0; i<scope.size; ++i ) {
  //   _min_ += scope[i].get_min();
  //   _max_ += scope[i].get_max();
  // }
  // if(_min_ > total )  gap = _min_-total;
  // else if(_max_ < total ) gap = _max_-total;

  // if(gap) {
  // // if(_min_ > total || _max_ < total) {
  //   wiped_idx = FAILURE(active[0]);
  // } else if(_min_ == total ) {
  //   for( i=0; i<scope.size; ++i ) 
  //     if( !scope[i].is_ground() ) scope[i].set_domain(0);
  // } else if(_max_ == total ) {
  //   for( i=0; i<scope.size; ++i ) 
  //     if( !scope[i].is_ground() ) scope[i].set_domain(1);
  // }


  return wiped_idx;
}

int Mistral::ConstraintBoolSumEqual::check( const int* s ) const 
{
  int i=scope.size, t=0;
  while(i--) t+=s[i];
  return total != t; 
}




// Mistral::Explanation::iterator Mistral::ConstraintBoolSumEqual::begin(Atom a) {
//   explanation.clear();
//   int i=scope.size;

//   if(a != NULL_ATOM) {
//     if(get_solver()->variables[a].get_value()) {
//       // the literal we try to explain is positive
//       while(i--) if(!(scope[i].get_max())) explanation.add(literal(scope[i]));
//     } else {
//       // the literal we try to explain is positive
//       while(i--) if(  scope[i].get_min() ) explanation.add(literal(scope[i]));
//     }
//   } else {

//     // for(i=0; i<scope.size; ++i) {
//     //   std::cout << scope[i] << " in " << scope[i].get_domain() << std::endl;
//     // }

//     // std::cout << "gap: " << gap << std::endl;

//     if(gap > 0) {
//       // too many ones
//       while(i-- && explanation.size <= total) {
// 	if(scope[i].get_min()) explanation.add(literal(scope[i]));
//       }
//     } else {
//       // too many zeros
//       while(i-- && explanation.size <= scope.size-total) {
// 	if(!(scope[i].get_max())) explanation.add(literal(scope[i]));
//       }
//     }


//     // std::cout << this << " has failed because of "
//     // 	      << explanation << std::endl;
//   }


//   return explanation.begin();
// }
// Mistral::Explanation::iterator Mistral::ConstraintBoolSumEqual::end  (Atom a) {
//   return explanation.end();
// }

void Mistral::ConstraintBoolSumEqual::initialise_activity(double *lact, double *vact, double norm) {

  int n = scope.size;
  int real_max = std::min(n, (int)max_);
  int real_min = std::max(0, (int)min_);
  int span = (real_max - real_min);

  double center = (double)(real_min + real_max) / 2;
  double skew = center/(double)(n);


  double activity_increment = norm / (scope.size * (1 << span));
	      
  int i=n;
  while(i--) {
    lact[2*scope[i].id()] += (1-skew) * activity_increment;
    lact[2*scope[i].id()+1] += skew * activity_increment;
    vact[scope[i].id()] += activity_increment;
  }
}


Mistral::Explanation::iterator Mistral::ConstraintBoolSumEqual::get_reason_for(const Atom a, const int lvl, Explanation::iterator& end) {

  // std::cout << "explain \n";
  // for(int k=0; k<scope.size; ++k) {
  //   std::cout << scope[k].get_domain() << " ";
  // }
  // std::cout << "\n" << gap << "\n" << std::endl;

  explanation.clear();
  int i=scope.size;

  if(a != NULL_ATOM) {
    if(get_solver()->variables[a].get_value()) {
      // the literal we try to explain is positive
      while(i--) {
	if( !(scope[i].get_max()) ) explanation.add(literal(scope[i], true));
      }
    } else {
      // the literal we try to explain is positive
      while(i--) {
	if(  scope[i].get_min() ) explanation.add(literal(scope[i], false));
      }
    }
  } else {
    //if(gap > 0) {
    if(min_>upper_bound) {
      // too many ones
      while(i-- && explanation.size <= upper_bound) {
	if(scope[i].get_min()) explanation.add(literal(scope[i], false));
      }
    } else {
      // too many zeros
      while(i-- && explanation.size <= scope.size-lower_bound) {
	if(!(scope[i].get_max())) explanation.add(literal(scope[i], true));
      }
    }
  }

  //  std::cout << explaantion << std::endl;

  end = explanation.end();
  return explanation.begin();
}


std::ostream& Mistral::ConstraintBoolSumEqual::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << " + " << scope[i]/*.get_var()*/;
  os << ") == " << total ;
  return os;
}

//#define _DEBUG_CARD true


Mistral::ConstraintBoolSumInterval::ConstraintBoolSumInterval(Vector< Variable >& scp, const int l, const int u)
  : GlobalConstraint(scp) { 
  priority = 1;
  lower_bound = l; 
  upper_bound = u; 
}

void Mistral::ConstraintBoolSumInterval::initialise() {
  ConstraintImplementation::initialise();

  int _min_ = 0;
  int _max_ = scope.size;
  for(unsigned int i=0; i<scope.size; ++i) {
    trigger_on(_VALUE_, scope[i]);
    if(scope[i].is_ground()) {
      if(scope[i].get_value()) ++_min_;
      else --_max_;
    }
  }

  min_.initialise(get_solver(), _min_);
  max_.initialise(get_solver(), _max_);

  GlobalConstraint::initialise();
}

void Mistral::ConstraintBoolSumInterval::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}

Mistral::ConstraintBoolSumInterval::~ConstraintBoolSumInterval() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete boolsuminterval constraint" << std::endl;
#endif
}

Mistral::PropagationOutcome Mistral::ConstraintBoolSumInterval::propagate() 
{
  PropagationOutcome wiped_idx = CONSISTENT;

#ifdef _DEBUG_CARD
  if(_DEBUG_CARD) {
    std::cout << "c" << id << ":\n";
    std::cout << "changes: " << changes << std::endl;
    std::cout << " events: " << events << std::endl;
    
    int new_evt[scope.size];
    for(int i=0; i<scope.size; ++i) {
      new_evt[i]=0;
    }
    for(int i=0; i<events.size; ++i) {
      new_evt[events[i]] = 1;
    }
    std::cout << " propagate [" ;
    for(int i=0; i<scope.size; ++i) {
      std::cout << std::setw(5) << scope[i].get_domain();
    }
    std::cout << "]\n   changes [" ;
    for(int i=0; i<scope.size; ++i) {
      std::cout << std::setw(5) << (new_evt[i] ? scope[i].get_domain() : " ");
    }
    std::cout << "]\n was in [" << min_ << ".." << max_ << "] now in [";
  }
#endif



  while(!changes.empty()) {
    if(LB_CHANGED(event_type[changes.pop()])) ++min_;
    else --max_;
  }

#ifdef _DEBUG_CARD
  if(_DEBUG_CARD) {
  std::cout << min_ << ".." << max_ << "]\n";
 }
#endif

  // if(min_>upper_bound || max_<lower_bound) {
  //   wiped_idx = FAILURE(events[0]);
  // } else if(min_==upper_bound) {
  //   // here there should not be any failure
  //   for(unsigned int i=0; i<active.size; ++i) {
  //     scope[active[i]].set_domain(0);
  //   }
  // } else if(max_==lower_bound) {
  //   // here there should not be any failure
  //   for(unsigned int i=0; i<active.size; ++i) {
  //     scope[active[i]].set_domain(1);
  //   }
  // }

  if(min_>upper_bound || max_<lower_bound) {
    
#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "FAILURE!\n";
    }
#endif

    wiped_idx = FAILURE(changes[0]);
  } else if(min_==upper_bound) {

#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "UPPER BOUND REACHED!\n";
    }
#endif

    // here there should not be any failure
    for(unsigned int i=0; i<active.size; ++i) {
      if(!(scope[active[i]].is_ground()) && FAILED(scope[active[i]].set_domain(0))) {
	wiped_idx = FAILURE(i);
	break;
      }
    }
  } else if(max_==lower_bound) {

#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "LOWER BOUND REACHED!\n";
    }
#endif

    // here there should not be any failure
    for(unsigned int i=0; i<active.size; ++i) {
      if(!(scope[active[i]].is_ground()) && FAILED(scope[active[i]].set_domain(1))) {
	wiped_idx = FAILURE(i);
	break;
      }
    }
  } else if(min_ >= lower_bound && max_ <= upper_bound) {

#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "ENTAILED!\n";
    }
#endif

    relax();
  }

#ifdef _DEBUG_CARD
  else {
    if(_DEBUG_CARD) {
      std::cout << "OK\n";
    }
  }
#endif

  return wiped_idx;


  // int _min_ = 0;
  // int _max_ = 0;
  // unsigned int i;

  // for( i=0; i<scope.size; ++i ) {
  //   _min_ += scope[i].get_min();
  //   _max_ += scope[i].get_max();
  // }
  // if(_min_ > upper_bound || _max_ < lower_bound) return FAILURE(active[0]);
  // else if(_min_ == upper_bound ) {
  //   for( i=0; i<scope.size; ++i ) 
  //     if( !scope[i].is_ground() ) scope[i].set_domain(0);
  // } else if(_max_ == lower_bound ) {
  //   for( i=0; i<scope.size; ++i ) 
  //     if( !scope[i].is_ground() ) scope[i].set_domain(1);
  // }
  // return CONSISTENT;
}

double factorial(const int n) {

  //std::cout << "factorial(" << n << ") = ";

  double fact = 1.0;
  for(int i=1; i<n;) fact *= ++i;

  //std::cout << fact << std::endl;

  return fact;
}

long double bi_coeff(const int n, const int k) {
  //std::cout << "(" << n << " choose " << k << ") = \n";

  double res = 1.0;  
  if(k) {

    int u;
    int o;
    if(k<n/2) u = k;
    else u = n-k;
    o = n-u;

    for(int i=o; i<n;) {
      
      //std::cout << " * " << (i+1) << " / " << u  << std::endl;
      
      res *= ++i;
      if(u>1) {
	res /= u--;
      }
      // if(o>1) {
      // 	res /= o--;
      // }
    }
  }

  //std::cout << " ==> " << res << std::endl;

  return res;

  /*

  double res = 0;
  if(k<n/2) {
    res = factorial(n, n-k) / factorial(k);
  } else {
    res = factorial(n, k) / factorial();
  }

  if(k==1 || k==n-1) return n;
  if(k==2 || k==n-2) return n*(n-1)/2;
  if(k==3 || k==n-3) return n*(n-1)*(n-2)/6;


  double res = factorial(n);

  std::cout << "res: " << res << std::endl;

  res /= factorial(k);

std::cout << "res: " << res << std::endl;

  res /= factorial(n-k);

std::cout << "res: " << res << std::endl;

  std::cout << res << std::endl;

  */

}


double sum_bi_coeff(const int nn, const int lb, const int ub) {

  int n = nn;
  int l = lb;
  int u = ub;
  int s = u-l+1;
  int d = 1;

  double res = 0;
  for(int i=1; i<=l; ++i)
    res += ((n-i+1) / i * s);
  

  for(int i=l; i<u; ++i)
    res += (n-i) / (i-1) * --s;

  return res;
}


void Mistral::ConstraintBoolSumInterval::initialise_activity(double *lact, double *vact, double norm) {

  int n = scope.size;
  int center = n/2;
  int real_max = std::min(n, upper_bound);
  int real_min = std::max(0, lower_bound);


  long double sol_0 = 0;
  long double sol_1 = 0;
  
  double incr_0 = 0;
  double incr_1 = 0;

  double total_weight = 0;  

  if(n<500) {

    long double total_asgn = pow(2.0, n);

    // std::cout << "arity: " << n << std::endl;
  
    // std::cout << real_min << " .. " << real_max << std::endl;

    // std::cout << "total_asgn: " << total_asgn << std::endl;

    long double total_sol = 0;
    for(int k=real_min; k<=real_max; ++k) {
      total_sol += bi_coeff(n, k);
    }
  
    //std::cout << "total_sol: " << total_sol << std::endl;

    // case x = 0;
  
    for(int k=real_min; k<=real_max; ++k) {
      sol_0 += bi_coeff(n-1, k);
    }

    // double sbc = sum_bi_coeff(n-1, real_min, real_max);

    // if(sbc != sol_0) {
    //   std::cout << sol_0 << " <1> " << sbc << std::endl;
    //   exit(1);
    // }


    //std::cout << "#sol[x=0]: " << sol_0 << std::endl;

    // // case x = 1;
    // double sol_1 = 0;
    // for(int k=real_min-1; k<real_max; ++k) {
    //   sol_1 += bi_coeff(n-1, k);
    // }

    // case x = 1;
    sol_1 = sol_0 - bi_coeff(n-1, real_max) + bi_coeff(n-1, real_min-1);
  

    // double ssol_1 = 0;
    // for(int k=real_min-1; k<real_max; ++k) {
    //   ssol_1 += bi_coeff(n-1, k);
    // }

    // std::cout << "#sol[x=1]: " << sol_1 << std::endl;

    // if(ssol_1 != sol_1) {
    //   std::cout << " should have been " << ssol_1 << "!!\n";
    //   exit(1);
    // }

    double ratio_0 = 2*(sol_0/total_asgn);
    double ratio_1 = 2*(sol_1/total_asgn);

    //std::cout << "ratio #sol[x=0]: " << ratio_0 << " weight: " << (1-ratio_0) << std::endl;

    //std::cout << "ratio #sol[x=1]: " << ratio_1 << " weight: " << (1-ratio_1) << std::endl;
  
    double incr_0 = norm*(1-ratio_0);
    double incr_1 = norm*(1-ratio_1);

    // std::cout << "ratio #sol[x=0]: " << ratio_0 << " weight: " << incr_0 << std::endl;

    // std::cout << "ratio #sol[x=1]: " << ratio_1 << " weight: " << incr_1 << std::endl;


    total_weight = incr_1+incr_0;
    // 
    incr_1 += total_weight * (sol_0/(sol_0+sol_1));
    incr_0 += total_weight * (sol_1/(sol_0+sol_1));
    total_weight *=2;  
  }


  total_weight = incr_1+incr_0;


	      
  int i=n, idx;
  while(i--) {
    idx = scope[i].id();
    lact[2*idx] += incr_0;
    lact[2*idx+1] += incr_1;
    vact[idx] += total_weight;
  }
}


Mistral::Explanation::iterator Mistral::ConstraintBoolSumInterval::get_reason_for(const Atom a, const int lvl, Explanation::iterator& end) {

  // std::cout << "explain \n";
  // for(int k=0; k<scope.size; ++k) {
  //   std::cout << scope[k].get_domain() << " ";
  // }
  // std::cout << "\n" << gap << "\n" << std::endl;

  explanation.clear();
  int i=scope.size;

  if(a != NULL_ATOM) {
    if(get_solver()->variables[a].get_value()) {
      // the literal we try to explain is positive
      while(i--) {
	if( !(scope[i].get_max()) ) explanation.add(literal(scope[i], true));
      }
    } else {
      // the literal we try to explain is positive
      while(i--) {
	if(  scope[i].get_min() ) explanation.add(literal(scope[i], false));
      }
    }
  } else {
    if(min_>upper_bound) {
      // too many ones
      while(i-- && explanation.size <= upper_bound) {
	if(scope[i].get_min()) explanation.add(literal(scope[i], false));
      }
    } else {
      // too many zeros
      while(i-- && explanation.size <= scope.size-lower_bound) {
	if(!(scope[i].get_max())) explanation.add(literal(scope[i], true));
      }
    }
  }

  //std::cout << explanation << std::endl;

  end = explanation.end();
  return explanation.begin();
}

int Mistral::ConstraintBoolSumInterval::check( const int* s ) const 
{
  int i=scope.size, t=0;
  while(i--) t+=s[i];
  return (t<lower_bound || t>upper_bound); 
}

std::ostream& Mistral::ConstraintBoolSumInterval::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << " + " << scope[i]/*.get_var()*/;
  if(lower_bound == -INFTY)
    os << ") <= " << upper_bound ;
  else if(upper_bound == INFTY)
    os << ") >= " << lower_bound ;
  else 
    os << ") in [" << lower_bound << "," << upper_bound << "]" ;
  return os;
}



Mistral::PredicateBoolSum::PredicateBoolSum(Vector< Variable >& scp, Variable tot)
  : GlobalConstraint(scp) { 
  scope.add(tot);
  priority = 1;
}

Mistral::PredicateBoolSum::PredicateBoolSum(std::vector< Variable >& scp, Variable tot)
  : GlobalConstraint(scp) {
  scope.add(tot);
  priority = 1;
}

Mistral::PredicateBoolSum::PredicateBoolSum(Vector< Variable >& scp)
  : GlobalConstraint(scp) { 
  priority = 1;
}

Mistral::PredicateBoolSum::PredicateBoolSum(std::vector< Variable >& scp)
  : GlobalConstraint(scp) { 
  priority = 1;
}

void Mistral::PredicateBoolSum::initialise() {
  ConstraintImplementation::initialise();
  for(unsigned int i=0; i<scope.size-1; ++i) {
    trigger_on(_VALUE_, scope[i]);
  }
  trigger_on(_RANGE_, scope[scope.size-1]);
  GlobalConstraint::initialise();
}

void Mistral::PredicateBoolSum::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}

Mistral::PredicateBoolSum::~PredicateBoolSum() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete boolsum predicate" << std::endl;
#endif
}

Mistral::PropagationOutcome Mistral::PredicateBoolSum::propagate() 
{
  int _min_ = 0;
  int _max_ = 0;
  unsigned int // _min_=0, _max_=0,
    i, n=scope.size-1;
  int lb, ub;

  for( i=0; i<n; ++i ) {
    _min_ += scope[i].get_min();
    _max_ += scope[i].get_max();
  }
  if(_min_ > scope[n].get_max() || _max_ < scope[n].get_min()) return FAILURE(n);
  else {
    scope[n].set_min(_min_);
    scope[n].set_max(_max_);
    lb = scope[n].get_min();
    ub = scope[n].get_max();

    if(_min_ == ub) {
      for( i=0; i<n; ++i ) 
	if( !scope[i].is_ground() ) scope[i].set_domain(0);
    } else if(_max_ == lb) {
      for( i=0; i<n; ++i ) 
	if( !scope[i].is_ground() ) scope[i].set_domain(1);
    }
  }
  return CONSISTENT;
}

int Mistral::PredicateBoolSum::check( const int* s ) const 
{
  int i=scope.size-1, t=0;
  while(i--) t+=s[i];
  return (t != s[scope.size-1]); 
}

std::ostream& Mistral::PredicateBoolSum::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size-1; ++i) 
    os << " + " << scope[i]/*.get_var()*/;
  os << ") = " << scope[scope.size-1];
  return os;
}




Mistral::PredicateWeightedSum::PredicateWeightedSum(Vector< Variable >& scp, 
						    const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<scope.size; ++i) {
    weight.add(1);
  }
}

Mistral::PredicateWeightedSum::PredicateWeightedSum(Vector< Variable >& scp, 
						    Vector< int >& wgt,
						    const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<scope.size; ++i) {
    weight.add(wgt[i]);
  }
}

Mistral::PredicateWeightedSum::PredicateWeightedSum(std::vector< Variable >& scp, 
						    std::vector< int >& wgt,
						    const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<scope.size; ++i) {
    weight.add(wgt[i]);
  }
}

void Mistral::PredicateWeightedSum::initialise() {
  ConstraintImplementation::initialise();
  //set_idempotent(true);
  //set_idempotent(false);

  wpos = 0;
  wneg = weight.size;

  // display(std::cout);
  // std::cout << std::endl;

  
  int aux_i;
  Variable aux_v;

  for(int i=0; i<wneg; ++i) {

    // std::cout << weight << std::endl;

    if(weight[i] == 1) { // swap i with wpos and increment wpos
      if(i>wpos) {
	weight[i] = weight[wpos];
	weight[wpos] = 1;
	
	aux_v = scope[i];
	scope[i] = scope[wpos];
	scope[wpos] = aux_v;

	--i;
      }
      ++wpos;
    } else if(weight[i] < 0) { // decrement wneg and swap i with wneg 
      --wneg;

      aux_i = weight[i];
      weight[i] = weight[wneg];
      weight[wneg] = aux_i;

      aux_v = scope[i];
      scope[i] = scope[wneg];
      scope[wneg] = aux_v;

      --i;
    }
  }

  for(unsigned int i=0; i<scope.size; ++i)
    trigger_on(_RANGE_, scope[i]);
  GlobalConstraint::initialise();



  lo_bound = new int[scope.size];
  up_bound = new int[scope.size];
  span = new int[scope.size];

  // //std::cout << (int*)env << std::endl;
  // std::cout  << "-- " << (int*)solver << std::endl;

  // exit(1);


  unknown_parity.initialise(solver, 0, scope.size-1, scope.size, true);
  //parity.Reversible::initialise(scope[0].get_solver());
  parity.initialise(solver, ((lower_bound%2)!=0));


  //std::cout << "--parity-- " << parity << std::endl;


  for(int i=0; i<wpos; ++i) {
    if( scope[i].is_ground() ) {
      // the parity  only if the only one val is odd
      if( scope[i].get_min()%2 ) parity = 1-parity;
      unknown_parity.reversible_remove(i);
    }
  }

  for(unsigned int i=wpos; i<scope.size; ++i) {
    if( weight[i]%2 == 0 )
      unknown_parity.reversible_remove(i);
    else if( scope[i].is_ground() ) {
      unknown_parity.reversible_remove(i);
      if( scope[i].get_min()%2 ) parity = 1-parity;
    }
  }

   // display(std::cout);
   // std::cout << std::endl;

  // exit(1);

}

void Mistral::PredicateWeightedSum::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}

// void Mistral::PredicateWeightedSum::change_weight(const int i, const int w) {
//   if(weight[i]%2 =w%2))

//   weight[i] = w;
// }

Mistral::PredicateWeightedSum::~PredicateWeightedSum() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete weightedsum constraint" << std::endl;
#endif
  delete [] lo_bound;
  delete [] up_bound;
  delete [] span;
}




Mistral::PropagationOutcome Mistral::PredicateWeightedSum::rewrite() {

#ifdef _DEBUG_REWRITE
      std::cout << "REWRITE SUM " ;
      display(std::cout);
      std::cout << std::endl;
#endif

  RewritingOutcome r_evt = NO_EVENT; 


  //std::cout << scope.size << " " << wpos << " " << wneg << " " << lower_bound << " " << upper_bound << std::endl;

  // check if it can be rewritten as an ADD predicate
  if(scope.size == 3 && wpos == 1 && wneg == 1 && lower_bound == 0 && upper_bound == 0 ) {

    //std::cout <<  "RELAX" << std::endl;


    r_evt = SUPPRESSED;
    relax();
    get_solver()->add(Constraint(new PredicateAdd(scope[1], scope[2], scope[0])));
  }


  else if(scope.size == 2 && wpos == 1 && wneg == 1) {
    if(lower_bound == upper_bound) {
      r_evt = SUPPRESSED;
      relax();
      if(lower_bound == 0) {
	get_solver()->add(Constraint(new ConstraintEqual(scope[0], scope[1])));
      } else {
	get_solver()->add(Constraint(new PredicateOffset(scope[1], scope[0], lower_bound)));
      } 
    } else {
      if(upper_bound == INFTY) {
	r_evt = SUPPRESSED;
	relax();
	get_solver()->add(Constraint(new ConstraintLess(scope[1], scope[0], -lower_bound)));
      }
    }
  }
  
  return r_evt;
}



Mistral::PropagationOutcome Mistral::PredicateWeightedSum::propagate() 
{
  
  int i, j;
  // compute the max and th min
  int tmin, smin=0, tmax, smax=0// , maxspan=0
    , arity=scope.size;
  PropagationOutcome wiped = CONSISTENT;
  
#ifdef _DEBUG_WEIGHTEDSUM
  if(_DEBUG_WEIGHTEDSUM) {
    std::cout << std::endl << "propagate " << lower_bound << " <= " ;
    for(i=0; i<arity; ++i) {
      std::cout << " " << weight[i] << scope[i] << ":" << scope[i].get_domain();
    }
    std::cout << " <= " << upper_bound << std::endl << changes << std::endl;
  }
#endif
  
  for(i=0; i<wpos; ++i) {
    smax += (up_bound[i] = scope[i].get_max());
    smin += (lo_bound[i] = scope[i].get_min());
    span[i] = (up_bound[i]-lo_bound[i]);

#ifdef _DEBUG_WEIGHTEDSUM
    if(_DEBUG_WEIGHTEDSUM) {
      if(i)
	std::cout << " + [" << lo_bound[i] << "," << up_bound[i] << "] = [" << smin << "," << smax << "] ";
      else
	std::cout << "[" << smin << "," << smax << "] ";
    }
#endif
    
  }
  for(i=wpos; i<wneg; ++i) {
    smax += weight[i] * (up_bound[i] = scope[i].get_max());
    smin += weight[i] * (lo_bound[i] = scope[i].get_min());
    span[i] = weight[i] * (up_bound[i]-lo_bound[i]);
    
#ifdef _DEBUG_WEIGHTEDSUM
    if(_DEBUG_WEIGHTEDSUM) {
      if(i)
	std::cout << " + [" << lo_bound[i] << "," << up_bound[i] << "] = [" << smin << "," << smax << "] ";
      else
	std::cout << "[" << smin << "," << smax << "] ";
    }
#endif
    
  }
  for(i=wneg; i<arity; ++i) {
    smax += weight[i] * (lo_bound[i] = scope[i].get_min());
    smin += weight[i] * (up_bound[i] = scope[i].get_max());
    span[i] = weight[i] * (lo_bound[i]-up_bound[i]);
    
#ifdef _DEBUG_WEIGHTEDSUM
    if(_DEBUG_WEIGHTEDSUM) {
      if(i)
	std::cout << " + [" << lo_bound[i] << "," << up_bound[i] << "] = [" << smin << "," << smax << "] ";
      else
	std::cout << "[" << smin << "," << smax << "] ";
    }
#endif
  }

  
  while(IS_OK(wiped) && !events.empty()) {

#ifdef _DEBUG_WEIGHTEDSUM
    if(_DEBUG_WEIGHTEDSUM) {
      std::cout << "\nprocessing events: " << events << std::endl;
    }
#endif

    if(lower_bound == upper_bound) {
      j = events.size;
      while( j-- ) {
	i = events[j];

	//std::cout << i << ": " << (span[i]) << " " << (unknown_parity.contain(i)) << std::endl;


	if(span[i]==0 && unknown_parity.contain(i)) {
	  unknown_parity.reversible_remove(i);
	  if( lo_bound[i]%2 ) parity = 1-parity;
	}
      }
      
#ifdef _DEBUG_WEIGHTEDSUM
      if(_DEBUG_WEIGHTEDSUM) {
	display(std::cout);
	std::cout << std::endl << unknown_parity << ": " << (parity ? "odd" : "even") << std::endl;
      }
#endif   
      
      
      if(unknown_parity.size == 0) {

#ifdef _DEBUG_WEIGHTEDSUM
	if(_DEBUG_WEIGHTEDSUM) {
	  std::cout << "parity failure " << std::endl;
	}
#endif

	if(parity != 0) wiped = FAILURE(arity-1);
      } else if(unknown_parity.size == 1) { // it needs to be of parity "parity"
	i = unknown_parity[0];
	
#ifdef _DEBUG_WEIGHTEDSUM
	if(_DEBUG_WEIGHTEDSUM) {
	  std::cout << "parity pruning: " << (lo_bound[i]%2) << " " << parity << std::endl ;
	}
#endif
	
	while(IS_OK(wiped) && (lo_bound[i]%2==0) != (parity==0)) {
	  
#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " => ";
	  }
#endif
	  
	  tmin = lo_bound[i];
	  if( IS_FAIL(scope[i].set_min(++lo_bound[i])) ) wiped = FAILURE(i);
	  else {
	    lo_bound[i] = scope[i].get_min();
	    if(i<wneg) smin += ((lo_bound[i] - tmin)*weight[i]);
	    else smax += ((lo_bound[i] - tmin)*weight[i]);
	  }

#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << std::endl;
	  }
#endif
	  
	}
	
	while(IS_OK(wiped) && (up_bound[i]%2==0) != (parity==0)) {

#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " => ";
	  }
#endif
	  
	  tmin = up_bound[i];
	  if( IS_FAIL(scope[i].set_max(--up_bound[i])) ) wiped = FAILURE(i);
	  else {
	    up_bound[i] = scope[i].get_max();
	    if(i<wneg) smax -= ((tmin - up_bound[i])*weight[i]);
	    else smin -= ((tmin - up_bound[i])*weight[i]);
	  }

#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << std::endl;
	  }
#endif

	}
      }
    }

    if(IS_OK(wiped)) {
      
      events.clear();
      
#ifdef _DEBUG_WEIGHTEDSUM
      if(_DEBUG_WEIGHTEDSUM) {
	std::cout << " [" << smin << "," << smax << "]" << std::endl;
      }
#endif
      
      if( smax < lower_bound || smin > upper_bound ) wiped = FAILURE(arity-1);
      else {
	tmax = (smax - lower_bound);
	tmin = (upper_bound - smin);
	
	for(i=0; IS_OK(wiped) && i<wpos; ++i) {
	  
	  if( tmin < (up_bound[i] - lo_bound[i]) ) {

#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " <= " << (lo_bound[i] + tmin) << std::endl;
	  }
#endif
	    
	    if(IS_FAIL(scope[i].set_max( lo_bound[i] + tmin ))) wiped = FAILURE(i);
	    else {
	      
	      events.add(i);
	      event_type[i] = UB_EVENT;
	    
	    }
	  }
	}
	
	for(i=wpos; IS_OK(wiped) && i<wneg; ++i) {
	  	  
	  if( tmin < (up_bound[i] - lo_bound[i]) * weight[i] ) {

#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " <= " << (lo_bound[i] + tmin/weight[i]) << std::endl;
	  }
#endif
	  	    
	    if(IS_FAIL(scope[i].set_max( lo_bound[i] + tmin/weight[i] ))) wiped = FAILURE(i);
	    
	    else {
	      events.add(i);
	      event_type[i] = UB_EVENT;
	    }
	  }
	}
	
	for(i=wneg; IS_OK(wiped) && i<arity; ++i) {
	  
	  if( tmin < (lo_bound[i] - up_bound[i]) * weight[i] ) {

#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " >= " << (up_bound[i] + tmin/weight[i]) << std::endl;
	  }
#endif
	    
	    if(IS_FAIL(scope[i].set_min( up_bound[i] + tmin/weight[i] ))) wiped = FAILURE(i);
	    
	    else {
	      events.add(i);
	      event_type[i] = LB_EVENT;
	    }
	  }
	}
	
	for(i=0; IS_OK(wiped) && i<wpos; ++i) {
	  
	  if( tmax < (up_bound[i] - lo_bound[i]) ) {
	    
#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " >= " << (up_bound[i] - tmax) << std::endl;
	  }
#endif

	    if(IS_FAIL(scope[i].set_min( up_bound[i] - tmax ))) wiped = FAILURE(i);
	    
	    else {
	      
	      if(events.contain(i)) {
		event_type[i] |= LB_EVENT;
	      } else {
		events.add(i);
		event_type[i] = LB_EVENT;
	      }

	    }
	  }
	}
	
	for(i=wpos; IS_OK(wiped) && i<wneg; ++i) {
	  
	  if( tmax < (up_bound[i] - lo_bound[i]) * weight[i] ) {

#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " >= " << (up_bound[i] - tmax/weight[i]) << std::endl;
	  }
#endif
	    
	    if(IS_FAIL(scope[i].set_min( up_bound[i] - tmax/weight[i] ))) wiped = FAILURE(i);
	    
	    else {
	      
	      if(events.contain(i)) {
		event_type[i] |= LB_EVENT;
	      } else {
		events.add(i);
		event_type[i] = LB_EVENT;
	      }
	      
	    }
	  }
	}


	for(i=wneg; IS_OK(wiped) && i<arity; ++i) {
	  
	  if( tmax < (lo_bound[i] - up_bound[i]) * weight[i] ) {  
	    
#ifdef _DEBUG_WEIGHTEDSUM
	  if(_DEBUG_WEIGHTEDSUM) {
	    std::cout << scope[i] << " in " << scope[i].get_domain() << " <= " << (lo_bound[i] - tmax/weight[i]) << std::endl;
	  }
#endif

	    if(IS_FAIL(scope[i].set_max( lo_bound[i] - tmax/weight[i] ))) wiped = FAILURE(i);
	    
	    else {
	      
	      if(events.contain(i)) {
		event_type[i] |= UB_EVENT;
	      } else {
		events.add(i);
		event_type[i] = UB_EVENT;
	      }

	    }
	  }
	}
      }
      
      /// update smin and smax
      for(unsigned int j=0; IS_OK(wiped) && j<events.size; ++j) {
	i = events[j];
	if(i<wpos) {
	  if(LB_CHANGED(event_type[i])){ 
	    smin -= lo_bound[i];
	    lo_bound[i] = scope[i].get_min();
	    span[i] = (up_bound[i]-lo_bound[i]);
	    smin += lo_bound[i];
	  } 
	  if(UB_CHANGED(event_type[i])){ 
	    smax -= up_bound[i];
	    up_bound[i] = scope[i].get_max();
	    span[i] = (up_bound[i]-lo_bound[i]);
	    smax += up_bound[i];
	  }
	} else if(i<wneg) {
	  if(LB_CHANGED(event_type[i])){ 
	    smin -= (lo_bound[i] * weight[i]);
	    lo_bound[i] = scope[i].get_min();
	    span[i] = weight[i] * (up_bound[i]-lo_bound[i]);
	    smin += (lo_bound[i] * weight[i]);
	  } 
	  if(UB_CHANGED(event_type[i])){ 
	    smax -= (up_bound[i] * weight[i]);
	    up_bound[i] = scope[i].get_max();
	    span[i] = weight[i] * (up_bound[i]-lo_bound[i]);
	    smax += (up_bound[i] * weight[i]);
	  }
	} else {
	  if(LB_CHANGED(event_type[i])){ 
	    smax -= (lo_bound[i] * weight[i]);
	    lo_bound[i] = scope[i].get_min();
	    span[i] = weight[i] * (lo_bound[i]-up_bound[i]);
	    smax += (lo_bound[i] * weight[i]);
	  } 
	  if(UB_CHANGED(event_type[i])){ 
	    smin -= (up_bound[i] * weight[i]);
	    up_bound[i] = scope[i].get_max();
	    span[i] = weight[i] * (lo_bound[i]-up_bound[i]);
	    smin += (up_bound[i] * weight[i]);
	  }
	}
      }
    }
  }

#ifdef _DEBUG_WEIGHTEDSUM
  if(_DEBUG_WEIGHTEDSUM) {
    std::cout << "result: ";
    for(i=0; i<arity; ++i) {
      std::cout << " " << weight[i] << scope[i] << ":" << scope[i].get_domain();
    }
    std::cout << std::endl;
  }
#endif

  return wiped;
}

int Mistral::PredicateWeightedSum::check( const int* s ) const 
{
  int i=scope.size, t=0;
  while(i--) {
    t+=(weight[i]*s[i]);
  }
  return (t < lower_bound || t > upper_bound); 
}

std::ostream& Mistral::PredicateWeightedSum::display(std::ostream& os) const {
  if(lower_bound > -INFTY) 
    os << lower_bound << " <= " ;
  os << weight[0] << "*" << scope[0]/*.get_var()*/ ;

  for(unsigned int i=1; i<scope.size; ++i) 
    os << " + " << weight[i] << "*" << scope[i]/*.get_var()*/;
  
  if(upper_bound < INFTY) 
    os << " <= " << upper_bound;
 

  return os;
}







Mistral::ConstraintWeightedBoolSumInterval::ConstraintWeightedBoolSumInterval(Vector< Variable >& scp, 
									      const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<scope.size; ++i) {
    weight.add(1);
  }
}

Mistral::ConstraintWeightedBoolSumInterval::ConstraintWeightedBoolSumInterval(Vector< Variable >& scp, 
									      Vector< int >& wgt,
									      const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<scope.size; ++i) {
    weight.add(wgt[i]);
  }
}

Mistral::ConstraintWeightedBoolSumInterval::ConstraintWeightedBoolSumInterval(std::vector< Variable >& scp, 
									      std::vector< int >& wgt,
									      const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<scope.size; ++i) {
    weight.add(wgt[i]);
  }
}

void Mistral::ConstraintWeightedBoolSumInterval::initialise() {
  ConstraintImplementation::initialise();
  //set_idempotent(true);
  //set_idempotent(false);
  
  wpos = 0;
  wneg = weight.size;

   // display(std::cout);
   // std::cout << std::endl;

  
  int aux_i;
  Variable aux_v;

  for(int i=0; i<wneg; ++i) {

    //std::cout << weight << std::endl;

    if(weight[i] == 1) { // swap i with wpos and increment wpos
      if(i>wpos) {
	weight[i] = weight[wpos];
	weight[wpos] = 1;
	
	aux_v = scope[i];
	scope[i] = scope[wpos];
	scope[wpos] = aux_v;

	--i;
      }
      ++wpos;
    } else if(weight[i] < 0) { // decrement wneg and swap i with wneg 
      --wneg;

      aux_i = weight[i];
      weight[i] = weight[wneg];
      weight[wneg] = aux_i;

      aux_v = scope[i];
      scope[i] = scope[wneg];
      scope[wneg] = aux_v;

      --i;
    }
  }




  for(unsigned int i=0; i<scope.size; ++i)
    trigger_on(_RANGE_, scope[i]);
  GlobalConstraint::initialise();


  domains = new BoolDomain[scope.size];
  for(unsigned int i=0; i<scope.size; ++i) {
    Variable var = scope[i].get_var();
    domains[i] = var.bool_domain;

    //std::cout << domains[i] << " " << (*(domains[i])) << std::endl;
  }
  // lo_bound = new int[scope.size];
  // up_bound = new int[scope.size];
  // span = new int[scope.size];

  // //std::cout << (int*)env << std::endl;
  // std::cout  << "-- " << (int*)solver << std::endl;

  // exit(1);


  unknown_parity.initialise(solver, 0, scope.size-1, scope.size, true);
  //parity.Reversible::initialise(scope[0].get_solver());
  parity.initialise(solver, ((lower_bound%2)!=0));


  //std::cout << "--parity-- " << parity << std::endl;





  for(int i=0; i<wpos; ++i) {
    if( IS_GROUND(domains[i]) ) {// scope[i].is_ground() ) {
      // the parity  only if the only one val is odd
      if( GET_VAL(domains[i])%2 ) // scope[i].get_min()%2 ) 
	parity = 1-parity;
      unknown_parity.reversible_remove(i);
    }
  }

  for(unsigned int i=wpos; i<scope.size; ++i) {
    if( weight[i]%2 == 0 )
      unknown_parity.reversible_remove(i);
    else if( IS_GROUND(domains[i]) ) { //scope[i].is_ground() ) {
      unknown_parity.reversible_remove(i);
      if( GET_VAL(domains[i]) ) //scope[i].get_min()%2 ) 
	parity = 1-parity;
    }
  }

   // display(std::cout);
   // std::cout << std::endl;

  // exit(1);

}

void Mistral::ConstraintWeightedBoolSumInterval::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR|BITSET_VAR|RANGE_VAR);
}

// void Mistral::ConstraintWeightedBoolSumInterval::change_weight(const int i, const int w) {
//   if(weight[i]%2 =w%2))

//   weight[i] = w;
// }

Mistral::ConstraintWeightedBoolSumInterval::~ConstraintWeightedBoolSumInterval() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete weightedsum constraint" << std::endl;
#endif
  // delete [] lo_bound;
  // delete [] up_bound;
  // delete [] span;
  delete [] domains;
}




// Mistral::PropagationOutcome Mistral::ConstraintWeightedBoolSumInterval::rewrite() {

// #ifdef _DEBUG_REWRITE
//       std::cout << "REWRITE SUM " ;
//       display(std::cout);
//       std::cout << std::endl;
// #endif

//   RewritingOutcome r_evt = NO_EVENT; 
  
//   return r_evt;
// }





Mistral::Explanation::iterator Mistral::ConstraintWeightedBoolSumInterval::get_reason_for(const Atom a, const int lvl, Explanation::iterator& end) {
  /*
    get explanation for the pruning of variables[a] at level lvl.
    
    for each pruning, the constraint has stored the reason with the following code:
    1 -> parity 
    2 -> lb
    3 -> ub
    
    case 1: find the variables with odd coefficients that are not variables[a]. They are necessarily bound to prune a
    case 2: sum(max(w_x)) - w_a < lb ->
                                        find a set of x, assigned before lvl and such that sum(max(w_x)) - w_a < lb

					
in fact, only parity pruning must be stored. Otherwise, we can deduce it from the value of a (and the sign of its weight).

   */

  //std::cout << "^^^^^^^^^" << std::endl;
  /*
  if(a != NULL_ATOM) {
    std::cout << "extract a reason for " << get_solver()->variables[a] << " = " << get_solver()->variables[a].get_value() << " from: " << this << " with: " << std::endl;
  } else {
 std::cout << "extract a reason for failure of: " << this << " with: " << std::endl;
  }

  for(int i=0; i<scope.size; ++i) {
    std::cout << std::setw(5) << scope[i].get_domain();
  }
  std::cout << std::endl;
  for(int i=0; i<scope.size; ++i) {
    if(!(IS_GROUND(domains[i])) || (a == NULL_ATOM) || get_solver()->assignment_order.stack_[a] > get_solver()->assignment_order.stack_[scope[i].id()])
      std::cout << std::setw(5) << scope[i].get_domain();
    else
      std::cout << "{0,1}";
  }
  std::cout << std::endl;
  */

  //std::cout << 11 << std::endl;

  //std::cerr << 22 << std::endl;

  positive_contributors.clear();
  negative_contributors.clear();
  int i=scope.size, idx;
  int *rank = get_solver()->assignment_order.stack_;
  int a_rank = (a == NULL_ATOM ? INFTY-1 : rank[a]);
  bool direction;
  int bounds[2] = {0,0};
  int val, va=INFTY;

  Vector<int> pweight;
  Vector<int> nweight;
  Vector<int> pvar;
  Vector<int> nvar;

  //std::cerr << 33 << std::endl;
  
  while(i--) {
    idx = scope[i].id();

    //std::cout << a_rank << " <> " << rank[idx] << std::endl;

    if(idx == a) {
      va = i;
      // a=0 and coeff is positive or
      // a=1 and coeff is negative => 1 <=> pushed toward the lower value (positive_contributors are the reason)

      // a=1 and coeff is positive or
      // a=0 and coeff is negative => 0 <=> pushed toward the upper value (negative_contributors are the reason)
      direction = (GET_VAL(domains[i]) != weight[i]>0);

    } else if(IS_GROUND(domains[i]) && rank[idx] < a_rank) {


      val = GET_VAL(domains[i]);

      //std::cout << "ground: " << val << " " << weight[i] << " " << std::endl;

      if(val) {
	bounds[0] += weight[i];
	bounds[1] += weight[i];
      }

      if(val == weight[i]>0) {
	positive_contributors.add(NOT(literal(scope[i])));
	pweight.add(weight[i]);
	pvar.add(i);
      } else {
	negative_contributors.add(NOT(literal(scope[i])));
	nweight.add(weight[i]);
	nvar.add(i);
      }
    } else {
      if(weight[i] < 0) {
	bounds[0] += weight[i];
      } else {
	bounds[1] += weight[i];
      }
    }

    //std::cout << "+ " << weight[i] << "." << scope[i].get_domain() << ": [" << bounds[0] << "," << bounds[1] << "]\n";
    
  }

  //std::cerr << 44 << std::endl;

  if(a == NULL_ATOM) {
    if(bounds[0] > upper_bound) direction = true;
    else if(bounds[1] < lower_bound) direction = false;
    else {
      std::cout << "COULDN'T FIND THE CAUSE OF THE FAILURE!" << std::endl;
      exit(1);
    }
  }


  //std::cerr << 55 << std::endl;

  // if(direction) {

  //   int total = 0;
  //   for(i=0; i<positive_contributors.size; ++i) {
  //     total += pweight[i];
  //   }

  //   bool error = false;

  //   if(va != INFTY) { 
  //     int vala = GET_VAL(domains[va]);

  //     if(total + (1-vala)*weight[va] <= upper_bound || total + vala*weight[va] > upper_bound) {
  // 	std::cout << "ERROR!!" << std::endl;
  // 	error = true;
  //     }
  //   } else {

  //     if(total <= upper_bound)
  // 	{
  // 	  std::cout << "ERROR!!" << std::endl;
  // 	  error = true;
  // 	}
      
  //   }

  //   if(error) {
  //     std::cout << "pos: " << positive_contributors << std::endl;
      
  //     for(i=0; i<positive_contributors.size; ++i) {
  // 	std::cout << std::setw(5) << get_solver()->variables[positive_contributors[i]/2];
  //     }
  //     std::cout << std::endl;
      
  //     std::cout << " when all these guys are set in this way, we have " << (bounds[0]-total) << " + " << total ;
  //     if(va != INFTY) { 
  // 	if(GET_VAL(domains[va])) 
  // 	  std::cout << " + 0 > " << upper_bound << " hence we need " <<  scope[va] << " to be " << weight[va] << std::endl;
  // 	else
  // 	  std::cout << " + " << weight[va] << " > " << upper_bound << " hence we need " <<  scope[va] << " to be 0" << std::endl;
  //     } else {
	
  // 	std::cout << " hence we need one of them to be assigne differently" << std::endl;
	
  //     }

  //     exit(1);
  //   }
    
  // } else {

  //   int total = 0;
  //   for(i=0; i<negative_contributors.size; ++i) {
  //     total += nweight[i];
  //   }

  //   bool error = false;

  //   if(va != INFTY) { 
  //     int vala = GET_VAL(domains[va]);

  //     if(total + (1-vala)*weight[va] >= lower_bound || total + vala*weight[va] < lower_bound) {
  // 	std::cout << "ERROR!!" << std::endl;
  // 	error = true;
  //     }
  //   } else {

  //     if(total >= lower_bound)
  // 	{
  // 	  std::cout << "ERROR!!" << std::endl;
  // 	  error = true;
  // 	}
      
  //   }

  //   if(error) {
  //     std::cout << "neg: " << negative_contributors << std::endl;
      
  //     for(i=0; i<negative_contributors.size; ++i) {
  // 	std::cout << std::setw(5) << get_solver()->variables[negative_contributors[i]/2];
  //     }
  //     std::cout << std::endl;
      
  //     std::cout << " when all these guys are set in this way, we have " << (bounds[1]-total) << " + " << total ;
  //     if(va != INFTY) { 
  // 	if(GET_VAL(domains[va])) 
  // 	  std::cout << " + 0 < " << lower_bound << " hence we need " <<  scope[va] << " to be " << weight[va] << std::endl;
  // 	else
  // 	  std::cout << " + " << weight[va] << " < " << lower_bound << " hence we need " <<  scope[va] << " to be 0" << std::endl;
  //     } else {
	
  // 	std::cout << " hence we need one of them to be assigne differently" << std::endl;
	
  //     }
  //     exit(1);
  //   }
  // }


  //std::cerr << 66 << std::endl;  

  // /************** TEST THE NOGOOD *************/

  // Solver s;
  // VarArray X(scope.size, 0, 1);

  // s.add( BoolSum(X, weight, lower_bound, upper_bound) );

  // if(direction) {
  //   for(i=0; i<pvar.size; ++i) {
      
  //     if(!(scope[pvar[i]].is_ground())) {
  // 	std::cout << "NOGOOD INVOLVES AN OPEN VARIABLE!" << std::endl;
  // 	exit(1);
  //     } else if(rank[scope[pvar[i]].id()] >= a_rank) {
  // 	std::cout << "NOGOOD INVOLVES A VARIABLE FIXED LATER THAN THE ATOM!" << std::endl;
  // 	exit(1);
  //     }

  //     s.add( X[pvar[i]] == scope[pvar[i]].get_value() );
  //   }
    
  // } else {
  //  for(i=0; i<nvar.size; ++i) {
      
  //     if(!(scope[nvar[i]].is_ground())) {
  // 	std::cout << "NOGOOD INVOLVES AN OPEN VARIABLE!" << std::endl;
  // 	exit(1);
  //     } else if(rank[scope[nvar[i]].id()] >= a_rank) {
  // 	std::cout << "NOGOOD INVOLVES A VARIABLE FIXED LATER THAN THE ATOM!" << std::endl;
  // 	exit(1);
  //     }

  //     s.add( X[nvar[i]] == scope[nvar[i]].get_value() );
  //   }
    
  // }
  
  // if(a != NULL_ATOM) {
  //   s.add( X[va] != scope[va].get_value() );
  // }

  // s.consolidate();

  // Outcome result = s.depth_first_search(X);

  // if(result != UNSAT) {

  //   std::cout << "NOGOOD IS WRONG!" << std::endl;
  //   exit(1);

  // }



  // /************** TEST THE NOGOOD *************/


  // std::cout << " reason for linear equation failure: ";
  // if(direction) {
    
  //   print_literal(std::cout, positive_contributors[0]);

  //   for(i=1; i<positive_contributors.size; ++i) {
  //     std::cout << " v ";
  //     print_literal(std::cout, positive_contributors[i]);
  //   }
 
  // } else {

  //   print_literal(std::cout, negative_contributors[0]);

  //   for(i=1; i<negative_contributors.size; ++i) {
  //     std::cout << " v ";
  //     print_literal(std::cout, negative_contributors[i]);
  //   }

  // }

  // std::cout << std::endl;



  //nexit(1);

  end = (direction ? positive_contributors.end() : negative_contributors.end());

  //std::cout << 77 << std::endl;
  //std::cout << "vvvvvvvvv" << std::endl;

  return (direction ? positive_contributors.begin() : negative_contributors.begin());
}


//#define _DEBUG_WEIGHTEDBOOLSUM (id == 102)

Mistral::PropagationOutcome Mistral::ConstraintWeightedBoolSumInterval::propagate() 
{
  
  int i, j;
  // compute the max and th min
  int tmin, smin=0, tmax, smax=0// , maxspan=0
    , arity=scope.size;
  PropagationOutcome wiped = CONSISTENT;
  
#ifdef _DEBUG_WEIGHTEDBOOLSUM
  if(_DEBUG_WEIGHTEDBOOLSUM) {
    std::cout << std::endl << "propagate " << lower_bound << " <= " ;
    for(i=0; i<arity; ++i) {
      std::cout << " " << weight[i] << scope[i] << ":" << scope[i].get_domain();
    }
    std::cout << " <= " << upper_bound << std::endl << changes << std::endl;
  }
#endif
  
  for(i=0; i<wpos; ++i) {
    smax += GET_MAX(domains[i]); //(up_bound[i] = scope[i].get_max());
    smin += GET_MIN(domains[i]); //(lo_bound[i] = scope[i].get_min());
    //span[i] = (up_bound[i]-lo_bound[i]);

#ifdef _DEBUG_WEIGHTEDBOOLSUM
    if(_DEBUG_WEIGHTEDBOOLSUM) {
      if(i)
	std::cout << " + [" << GET_MIN(domains[i]) << "," << GET_MAX(domains[i]) << "] = [" << smin << "," << smax << "] ";
      else
	std::cout << "[" << smin << "," << smax << "] ";
    }
#endif
    
  }
  for(i=wpos; i<wneg; ++i) {
    smax += weight[i] * GET_MAX(domains[i]); //(up_bound[i] = scope[i].get_max());
    smin += weight[i] * GET_MIN(domains[i]); //(lo_bound[i] = scope[i].get_min());
    //span[i] = weight[i] * (up_bound[i]-lo_bound[i]);
    
#ifdef _DEBUG_WEIGHTEDBOOLSUM
    if(_DEBUG_WEIGHTEDBOOLSUM) {
      if(i)
	std::cout << " + [" << GET_MIN(domains[i]) << "," << GET_MAX(domains[i]) << "] = [" << smin << "," << smax << "] ";
      else
	std::cout << "[" << smin << "," << smax << "] ";
    }
#endif
    
  }
  for(i=wneg; i<arity; ++i) {
    smax += weight[i] * GET_MIN(domains[i]); //(lo_bound[i] = scope[i].get_min());
    smin += weight[i] * GET_MAX(domains[i]); //(up_bound[i] = scope[i].get_max());
    //span[i] = weight[i] * (lo_bound[i]-up_bound[i]);
    
#ifdef _DEBUG_WEIGHTEDBOOLSUM
    if(_DEBUG_WEIGHTEDBOOLSUM) {
      if(i)
	std::cout << " + [" << GET_MIN(domains[i]) << "," << GET_MAX(domains[i]) << "] = [" << smin << "," << smax << "] ";
      else
	std::cout << "[" << smin << "," << smax << "] ";
    }
#endif
  }

  
  while(IS_OK(wiped) && !events.empty()) {

#ifdef _DEBUG_WEIGHTEDBOOLSUM
    if(_DEBUG_WEIGHTEDBOOLSUM) {
      std::cout << "\nprocessing events: " << events << std::endl;
    }
#endif

    if(lower_bound == upper_bound) {

#ifdef _DEBUG_WEIGHTEDBOOLSUM
      if(_DEBUG_WEIGHTEDBOOLSUM) {
	std::cout << "parity check" << std::endl;
	display(std::cout);
	std::cout << std::endl << unknown_parity << ": " << (parity ? "odd" : "even") << std::endl;
      }
#endif   


      j = events.size;
      while( j-- ) {
	i = events[j];

	//std::cout << i << ": " << (int)(*(domains[i])) << " " << (unknown_parity.contain(i)) << std::endl;


	if(IS_GROUND(domains[i]) && unknown_parity.contain(i)) {
	  unknown_parity.reversible_remove(i);
	  if( GET_VAL(domains[i]) ) { //lo_bound[i]%2 )  

#ifdef _DEBUG_WEIGHTEDBOOLSUM
      if(_DEBUG_WEIGHTEDBOOLSUM) {
	    std::cout << weight[i] << " * " << scope[i] << ":" << scope[i].get_domain() << " is odd, so the parity changes\n";
      }
#endif   

	    parity = 1-parity;

	    
	  }
	}
      }
      
#ifdef _DEBUG_WEIGHTEDBOOLSUM
      if(_DEBUG_WEIGHTEDBOOLSUM) {
	display(std::cout);
	std::cout << std::endl << unknown_parity << ": " << (parity ? "odd" : "even") << std::endl;
      }
#endif   
      
      
      if(unknown_parity.size == 0) {

// #ifdef _DEBUG_WEIGHTEDBOOLSUM
// 	if(_DEBUG_WEIGHTEDBOOLSUM) {
	  std::cout << "parity failure " << std::endl;
// 	}
// #endif

	if(parity != 0) wiped = FAILURE(arity-1);
      } else if(unknown_parity.size == 1) { // it needs to be of parity "parity"
	i = unknown_parity[0];
	
// #ifdef _DEBUG_WEIGHTEDBOOLSUM
// 	if(_DEBUG_WEIGHTEDBOOLSUM) {
	  std::cout << "parity pruning: " << (GET_MIN(domains[i])%2) << " " << parity << std::endl ;
// 	}
// #endif
       
	  
#ifdef _DEBUG_WEIGHTEDBOOLSUM
	if(_DEBUG_WEIGHTEDBOOLSUM) {
	  std::cout << scope[i] << " in " << scope[i].get_domain() << " => ";
	}
#endif

	if( IS_FAIL(scope[i].set_domain(parity)) ) {
	  wiped = FAILURE(i);
	} else {
	  if(i<wpos) {
	    if(parity) ++smin;
	    else --smax;
	  } else if(i<wneg) {
	    if(parity) smin += weight[i];
	    else smax -= weight[i];
	  } else {
	    if(parity) smax += weight[i];
	    else smin -= weight[i];
	  }
	}
	
#ifdef _DEBUG_WEIGHTEDBOOLSUM
	if(_DEBUG_WEIGHTEDBOOLSUM) {
	  std::cout << scope[i] << " in " << scope[i].get_domain() << std::endl;
	}
#endif
	
      }
    }


    if(IS_OK(wiped)) {
      
      events.clear();
      
#ifdef _DEBUG_WEIGHTEDBOOLSUM
      if(_DEBUG_WEIGHTEDBOOLSUM) {
	std::cout << " [" << smin << "," << smax << "]" << std::endl;
      }
#endif
      
      if( smax < lower_bound || smin > upper_bound ) wiped = FAILURE(arity-1);
      else {
	tmax = (smax - lower_bound);
	tmin = (upper_bound - smin);
	
	for(i=0; IS_OK(wiped) && i<wpos; ++i) {
	  
	  if( !(tmin || IS_GROUND(domains[i])) ) {
	    // no freedom at all with the upper bound
	    
	    if(IS_FAIL(scope[i].set_domain( 0 ))) wiped = FAILURE(i);
	    else {
	      
	      events.add(i);
	      event_type[i] = UB_EVENT;
	    
	    }
	  }
	}
	
	for(i=wpos; IS_OK(wiped) && i<wneg; ++i) {
	  	  
	  if( tmin < (IS_FREE(domains[i])) * weight[i] ) {
	  	   
	    if(IS_FAIL(scope[i].set_domain( 0 ))) wiped = FAILURE(i); 
	    
	    else {
	      events.add(i);
	      event_type[i] = UB_EVENT;
	    }
	  }
	}
	
	for(i=wneg; IS_OK(wiped) && i<arity; ++i) {
	  
	  if( tmin < -(IS_FREE(domains[i])) * weight[i] ) {

	    if(IS_FAIL(scope[i].set_domain( 1 ))) wiped = FAILURE(i);
	    
	    else {
	      events.add(i);
	      event_type[i] = LB_EVENT;
	    }
	  }
	}
	
	for(i=0; IS_OK(wiped) && i<wpos; ++i) {
	  
	  if( !(tmax || (IS_GROUND(domains[i]))) ) {
	    
	    if(IS_FAIL(scope[i].set_domain( 1 ))) wiped = FAILURE(i);
	    
	    else {
	      
	      if(events.contain(i)) {
		event_type[i] |= LB_EVENT;
	      } else {
		events.add(i);
		event_type[i] = LB_EVENT;
	      }

	    }
	  }
	}
	
	for(i=wpos; IS_OK(wiped) && i<wneg; ++i) {
	  
	  if( tmax < (IS_FREE(domains[i])) * weight[i] ) {
	    
	    if(IS_FAIL(scope[i].set_domain( 1 ))) wiped = FAILURE(i);
	    
	    else {
	      
	      if(events.contain(i)) {
		event_type[i] |= LB_EVENT;
	      } else {
		events.add(i);
		event_type[i] = LB_EVENT;
	      }
	      
	    }
	  }
	}


	for(i=wneg; IS_OK(wiped) && i<arity; ++i) {
	  
	  if( tmax < -(IS_FREE(domains[i])) * weight[i] ) {  
	    
	    if(IS_FAIL(scope[i].set_domain( 0 ))) wiped = FAILURE(i);
	    
	    else {
	      
	      if(events.contain(i)) {
		event_type[i] |= UB_EVENT;
	      } else {
		events.add(i);
		event_type[i] = UB_EVENT;
	      }

	    }
	  }
	}
      }
      
      /// update smin and smax
      for(unsigned int j=0; IS_OK(wiped) && j<events.size; ++j) {
	i = events[j];
	if(i<wpos) {
	  if(LB_CHANGED(event_type[i])){ 
	    ++smin;
	  } 
	  if(UB_CHANGED(event_type[i])){ 
	    --smax;
	  }
	} else if(i<wneg) {
	  if(LB_CHANGED(event_type[i])){ 
	    smin += weight[i];
	  } 
	  if(UB_CHANGED(event_type[i])){ 
	    smax -= weight[i];
	  }
	} else {
	  if(LB_CHANGED(event_type[i])){ 
	    smax += weight[i];
	  } 
	  if(UB_CHANGED(event_type[i])){ 
	    smin -= weight[i];
	  }
	}
      }
    }
  }

#ifdef _DEBUG_WEIGHTEDBOOLSUM
  if(_DEBUG_WEIGHTEDBOOLSUM) {
    std::cout << "result: ";
    for(i=0; i<arity; ++i) {
      std::cout << " " << weight[i] << scope[i] << ":" << scope[i].get_domain();
    }
    std::cout << std::endl;
  }
#endif

  return wiped;
}

int Mistral::ConstraintWeightedBoolSumInterval::check( const int* s ) const 
{
  int i=scope.size, t=0;
  while(i--) {
    t+=(weight[i]*s[i]);
  }
  return (t < lower_bound || t > upper_bound); 
}

std::ostream& Mistral::ConstraintWeightedBoolSumInterval::display(std::ostream& os) const {
  if(lower_bound > -INFTY) 
    os << lower_bound << " <= " ;
  os << weight[0] << "*" << scope[0]/*.get_var()*/ ;

  for(unsigned int i=1; i<scope.size; ++i) 
    os << " + " << weight[i] << "*" << scope[i]/*.get_var()*/;
  
  if(upper_bound < INFTY) 
    os << " <= " << upper_bound;
 

  return os;
}




//#define _DEBUG_IWEIGHTEDBOOLSUM (id == 1)


Mistral::ConstraintIncrementalWeightedBoolSumInterval::ConstraintIncrementalWeightedBoolSumInterval(Vector< Variable >& scp, 
												    const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<scope.size; ++i) {
    weight.add(1);
  }
}

Mistral::ConstraintIncrementalWeightedBoolSumInterval::ConstraintIncrementalWeightedBoolSumInterval(Vector< Variable >& scp, 
												    Vector< int >& wgt,
												    const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<wgt.size; ++i) {
    weight.add(wgt[i]);
  }
}

Mistral::ConstraintIncrementalWeightedBoolSumInterval::ConstraintIncrementalWeightedBoolSumInterval(std::vector< Variable >& scp, 
												    std::vector< int >& wgt,
												    const int L, const int U)
  : GlobalConstraint(scp), lower_bound(L), upper_bound(U) { 
  priority = 1;
  for(unsigned int i=0; i<wgt.size(); ++i) {
    weight.add(wgt[i]);
  }
}

int *sort_array;
int increasing_weight(const void *x, const void *y) {
  int _x = std::abs(sort_array[*(int*)x]);
  int _y = std::abs(sort_array[*(int*)y]);
  if(_x < _y) {
    return -1;
  } else if(_x > _y) {
    return 1;
  }
  return 0;
}



//#define _DEBUG_IWEIGHTEDBOOLSUM (id == 75)


void Mistral::ConstraintIncrementalWeightedBoolSumInterval::initialise() {
  ConstraintImplementation::initialise();


  //std::cout << "INITIALISE C" << id << std::endl; 
  

  int ordering[weight.size];
  for(unsigned int i=0; i<weight.size; ++i) {
    ordering[i] = i;
  }

  sort_array = weight.stack_;
  qsort(ordering, weight.size, sizeof(int), increasing_weight);

  Vector<Variable> X;//(weight.size);
  Vector<int> w;//(weight.size);
  for(unsigned int i=0; i<weight.size; ++i) {
     X.add(scope[ordering[i]]);
     w.add(weight[ordering[i]]);
  }


  //  std::cout << X << std::endl << w << "\n";


  int _min_ = 0;
  int _max_ = 0;
  for(unsigned int i=0; i<weight.size; ++i) {
    scope[i] = X[i];
    weight[i] = w[i];


    trigger_on(_VALUE_, scope[i]);

    if(weight[i]<0) {
      _min_ += weight[i];
    } else {
      _max_ += weight[i];
    }

// #ifdef _DEBUG_IWEIGHTEDBOOLSUM
//     if(_DEBUG_IWEIGHTEDBOOLSUM) {
//      std::cout << "COMPUTE INITIAL BOUNDS: [" << _min_ << ".." << _max_ << "]\n";
//     }
// #endif


    // if(scope[i].is_ground()) {
    //   if(scope[i].get_value()) {
    // 	if(weight[i]<0) {
    // 	  // lose the possibility of a negative weight -> increase min
    // 	  _min_-=weight[i];
    // 	} else {
    // 	  // lose the possibility of a positive weight -> decrease max
    // 	  _max_-=weight[i];
    // 	}
    //   } else {
    // 	if(weight[i]<0) {
    // 	  // lose the possibility of not chosing a negative weight -> decrease max
    // 	  _max_+=weight[i];
    // 	} else {
    // 	  // lose the possibility of not chosing a positive weight -> increase min
    // 	  _min_+=weight[i];
    // 	}	
    //   }
    // }
  }

  //std::cout << scope << std::endl << weight << "\n";

  //min_index.initialise(get_solver(), 0);
  index_.initialise(get_solver(), weight.size-1);
  bound_[0].initialise(get_solver(), _min_);
  bound_[1].initialise(get_solver(), _max_);




  GlobalConstraint::initialise();


  domains = new BoolDomain[weight.size];
  for(unsigned int i=0; i<weight.size; ++i) {
    Variable var = scope[i].get_var();
    domains[i] = var.bool_domain;
  }

}

void Mistral::ConstraintIncrementalWeightedBoolSumInterval::mark_domain() {
  for(int i=weight.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR|BITSET_VAR|RANGE_VAR);
}

// void Mistral::ConstraintIncrementalWeightedBoolSumInterval::change_weight(const int i, const int w) {
//   if(weight[i]%2 =w%2))

//   weight[i] = w;
// }

Mistral::ConstraintIncrementalWeightedBoolSumInterval::~ConstraintIncrementalWeightedBoolSumInterval() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete weightedsum constraint" << std::endl;
#endif
  // delete [] lo_bound;
  // delete [] up_bound;
  // delete [] span;
  delete [] domains;
}


//#define _DEBUG_REASONIWBS (id == 2)


void Mistral::ConstraintIncrementalWeightedBoolSumInterval::initialise_activity(double *lact, double *vact, double norm) {
  
  int n = scope.size;
  
  double avg_weight = (double)(bound_[1] - bound_[0] + 1)/(double)n;

  int real_max = std::min((int)(bound_[1]), upper_bound);
  int real_min = std::max((int)(bound_[0]), lower_bound);

  long double total_asgn = pow(2.0, n);

  int k;

  // std::cout << "arity: " << n << std::endl;

  // std::cout << "reachable [" << bound_[0] << " .. " << bound_[1] << "]\n"; 

  // std::cout << "bounds [" << lower_bound << " .. " << upper_bound << "]\n"; 

  // std::cout << "avg_weight: " << avg_weight << std::endl;

  // std::cout << "total_asgn: " << total_asgn << std::endl;

  // double total_sol = 0;
  //  k = (int)((double)(real_min - bound_[0])/avg_weight);
  // for(double val=real_min; val<=real_max; val+=avg_weight) {
  //   total_sol += bi_coeff(n, k);
  //   ++k;
  // }
  
  // std::cout << "total_sol: " << total_sol << std::endl;

  // case x = 0;

  long double sol_0 = 0;
   k = (int)((double)(real_min - bound_[0])/avg_weight);
   //int l = k;
   //int u = k;
  for(double val=real_min; val<=real_max; val+=avg_weight) {
    sol_0 += bi_coeff(n-1, k);
    //u = k;
    ++k;
  }


  // double sbc = sum_bi_coeff(n-1, l, u);

  // if(sbc != sol_0) {
  //   std::cout << sol_0 << " <2> " << sbc << std::endl;
  //   exit(1);
  // }


  //std::cout << "#sol[x=0]: " << sol_0 << std::endl;

  // case x = 1;
  long double sol_1 = 0;
   k = (int)((double)(real_min - bound_[0])/avg_weight)-1;
  for(double val=real_min; val<=real_max; val+=avg_weight) {
    sol_1 += bi_coeff(n-1, k);
    ++k;
  }

  //double sol_1 = sol_0 - bi_coeff(n-1, real_max) + bi_coeff(n-1, real_min-1);
 

  //std::cout << "#sol[x=1]: " << sol_1 << std::endl;

  double ratio_0 = 2*(sol_0/total_asgn);
  double ratio_1 = 2*(sol_1/total_asgn);

  // std::cout << "ratio #sol[x=0]: " << ratio_0 << " weight: " << (1-ratio_0) << std::endl;

  // std::cout << "ratio #sol[x=1]: " << ratio_1 << " weight: " << (1-ratio_1) << std::endl;
  
  double incr_0 = norm*(1-ratio_0);
  double incr_1 = norm*(1-ratio_1);

   // std::cout << "ratio #sol[x=0]: " << ratio_0 << " weight: " << incr_0 << std::endl;

   // std::cout << "ratio #sol[x=1]: " << ratio_1 << " weight: " << incr_1 << std::endl;
  
  if(incr_0 < 0.0001 && incr_1 < 0.0001) {
    incr_0 = 0.0001 * norm;
    incr_1 = 0.0001 * norm;
  }

  double total_weight = incr_1+incr_0;
  // 
  incr_1 += total_weight * (sol_0/(sol_0+sol_1));
  incr_0 += total_weight * (sol_1/(sol_0+sol_1));
  total_weight *= 2;
 
	      
  int i=n, idx;
  while(i--) {
    idx = scope[i].id();
    lact[2*idx] += incr_0 * (double)(weight[i]) / avg_weight;
    lact[2*idx+1] += incr_1 * (double)(weight[i]) / avg_weight;
    vact[idx] += total_weight * (double)(weight[i]) / avg_weight;
  }


  // std::cout << "HERE" << std::endl;

  // exit(1);


  /*
  // std::cerr << "initialise activity for " << this << std::endl;
  
  // std::cerr << lact << " " << vact << std::endl;

  int n = weight.size;
  int real_max = std::min((int)(bound_[1]), upper_bound);
  int real_min = std::max((int)(bound_[0]), lower_bound);

  //std::cerr << real_min << " .. " << real_max << std::endl;

  double span = (real_max - real_min);

  double center = (double)(real_min + real_max) / 2;
  double skew = center/(double)(bound_[1] - bound_[0] + 1);
  if(skew<0) skew = -skew;

  int log_n = 0, j=n;
  while(j) {
    j>>=1;
    ++log_n;
  }

  int exp = (int)(log_n * (span)/(double)(bound_[1] - bound_[0] + 1));


  std::cout << "exp: " << exp << std::endl;

  std::cout << "skew: " << skew << std::endl;

  double activity_increment = norm / (1 << exp);
  double ac_i; 

  std::cout << activity_increment << std::endl;


  int i=n;
  while(i--) {
    //std::cerr << i << std::endl;
    ac_i = activity_increment * (double)(std::abs(weight[i])) / (double)(std::abs(weight.back()));

    // std::cerr << 11111 << "  " << (scope[i]) << " " << (2*scope[i].id()+(weight[i]<0)) << std::endl ;

    // std::cerr << lact << " " << lact[(2*scope[i].id()+(weight[i]<0))] << std::endl;

    lact[2*scope[i].id()+(weight[i]<0)] += (1-skew) * ac_i;

    //std::cerr << 22222 << std::endl;

    lact[2*scope[i].id()+(weight[i]>=0)] += skew * ac_i;

    //std::cerr << 33333 << std::endl;

    vact[scope[i].id()] += ac_i;

    std::cout << scope[i] << " * " << weight[i] << ": " << ac_i << " (" << skew*ac_i << "/" << (1-skew)*ac_i << ")" << std::endl;
  }
  */
}


Mistral::Explanation::iterator Mistral::ConstraintIncrementalWeightedBoolSumInterval::get_reason_for(const Atom a, const int lvl, Explanation::iterator& end) {
 
#ifdef _DEBUG_REASONIWBS
  if(_DEBUG_REASONIWBS) {
  std::cout << "compute reason for " << a << " with respect to " << this << std::endl;
  }
#endif

  explanation.clear();
  int i, idx;
  int *rank = get_solver()->assignment_order.stack_;
  int a_rank = INFTY-1;
  Explanation **reason = get_solver()->reason_for.stack_;







  // direction is 1 iff the constraint pushed toward ub and 0 otherwise
  bool direction = 1; 

  if(a != NULL_ATOM) {
    a_rank = rank[a];
    // WE CANNOT USE ACTIVE, BECAUSE IT IS NOT UPDATED AFTER A FAILURE (YET)
    //for(i=active.size; i<weight.size; ++i) {
    //if(active[i] == a) {
    for(i=weight.size; i--;) {
      idx = scope[i].id();
      if(idx == a) {
	direction = (GET_VAL(domains[i]) == weight[i]>0);
	break;
      }
    }
  } else {
    direction = (bound_[1] < lower_bound);
  }




#ifdef _DEBUG_REASONIWBS
  if(_DEBUG_REASONIWBS) {
    
    if(direction) {
      std::cout << scope[i] << " was set to a high value (" << (GET_VAL(domains[i]) * weight[i]) << ") because otherwise the maximum would be too low " << a << std::endl;
    } else {
      std::cout << scope[i] << " was set to a low value (" << (GET_VAL(domains[i]) * weight[i]) << ") because otherwise the minimum would be too high" << a << std::endl;
    }
    for(i=0; i<weight.size; ++i) {
      
      idx = scope[i].id();
      std::cout << scope[i] << ": " << scope[i].get_domain() ;
      if( !(IS_GROUND(domains[i])) )
	std::cout << " [active]" ;
      else {
	std::cout << " rank: " << rank[idx] ;
	std::cout << " val: " << (GET_VAL(domains[i])) ;
	std::cout << " weight " << weight[i] ;
      }
      
      
      if(idx != a && IS_GROUND(domains[i]) && rank[idx] < a_rank)
	std::cout << " => IN!";
      else 
	std::cout << " => OUT!";
      
      std::cout << std::endl;
    }
  }
#endif




// #ifdef _DEBUG_REASONIWBS


// #endif


  // finds all the assignment that decreased the maximum reachable value
  i = weight.size;
  while(i--) {
    idx = scope[i].id();
    if(idx != a && IS_GROUND(domains[i]) && rank[idx] < a_rank) {
      if((GET_VAL(domains[i]) == weight[i]>0) != direction) {

	// if(reason[idx] == this) {
	//   std::cout << "!!!!" << std::endl;
	// }
	
	explanation.add(NOT(literal(scope[i])));
      }
    }
  }

  end = explanation.end();

#ifdef _DEBUG_REASONIWBS
  if(_DEBUG_REASONIWBS) {
  std::cout << explanation << std::endl;
  }
#endif

  return explanation.begin();
}




Mistral::PropagationOutcome Mistral::ConstraintIncrementalWeightedBoolSumInterval::propagate() 
{
  
  PropagationOutcome wiped_idx = CONSISTENT;
  int w, i;



#ifdef _DEBUG_IWEIGHTEDBOOLSUM
  if(_DEBUG_IWEIGHTEDBOOLSUM) {
    std::cout << std::endl << "propagate " << lower_bound << " <= " ;
    for(i=0; i<weight.size; ++i) {
      std::cout << " " << weight[i] << scope[i] << ":" << scope[i].get_domain();
    }
    std::cout << " <= " << upper_bound << std::endl << changes << std::endl;
  }
#endif


#ifdef _DEBUG_IWEIGHTEDBOOLSUM
    if(_DEBUG_IWEIGHTEDBOOLSUM) {
      std::cout << "\ncurrent bounds: [" << bound_[0] << ".." << bound_[1] << "]" << std::endl;
    }
#endif



  while(!changes.empty()) {
    i = changes.pop();
    w = weight[i]<0;
    

#ifdef _DEBUG_IWEIGHTEDBOOLSUM
    if(_DEBUG_IWEIGHTEDBOOLSUM) {
      std::cout << "\nprocessing events: " << event2str(event_type[i]) << " on " 
		<< scope[i] << " in " << scope[i].get_domain() << std::endl;
    }
#endif

    if(LB_CHANGED(event_type[i])) {
      bound_[w]+=weight[i];
    } else {
      bound_[1-w]-=weight[i];
    }    


#ifdef _DEBUG_IWEIGHTEDBOOLSUM
    if(_DEBUG_IWEIGHTEDBOOLSUM) {
      std::cout << "\ncurrent bounds: [" << bound_[0] << ".." << bound_[1] << "]" << std::endl;
    }
#endif

  }


#ifdef _DEBUG_IWEIGHTEDBOOLSUM
  if(_DEBUG_IWEIGHTEDBOOLSUM) {
    int real_min = 0;
    int real_max = 0;
    for(i=0; i<weight.size; ++i) {
      if(scope[i].is_ground()) {
	if(scope[i].get_value()) {
	  real_min += weight[i];
	  real_max += weight[i];
	}
      } else if(weight[i] < 0) {
	real_min += weight[i];
      } else {
	real_max += weight[i];
      }
    }

    if(real_min != bound_[0]) {
      std::cout << "(1) mismatch on lower bound: real = " << real_min << ", incremental = " << bound_[0] << std::endl;
      exit(1);
    }

    if(real_max != bound_[1]) {
      std::cout << "(1) mismatch on upper bound: real = " << real_max << ", incremental = " << bound_[1] << std::endl;
      exit(1);
    }

    if(_DEBUG_IWEIGHTEDBOOLSUM) {
      std::cout << "\ntarget bounds: [" << lower_bound << ".." << upper_bound << "]" << std::endl;
    }
  }
#endif

  
   
  if(bound_[0]>upper_bound || bound_[1]<lower_bound) {
    
#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
       std::cout << "FAILURE!\n";
    }
#endif
    
    wiped_idx = FAILURE(changes[0]);
   } else if(bound_[0] >= lower_bound && bound_[1] <= upper_bound) {
    
#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
      std::cout << "ENTAILED!\n";
    }
#endif

    relax();
  } else {
    i = index_;

    while(i>=0) {
      // 
      while(i>=0 && scope[i].is_ground()) --i;
   
      if(i>=0) {

#ifdef _DEBUG_IWEIGHTEDBOOLSUM
      if(_DEBUG_IWEIGHTEDBOOLSUM) {
	std::cout << "\nnext index: "<< i << ": " << scope[i] << " in " << scope[i].get_domain() 
		  << " * " << weight[i] << std::endl;
      }
#endif


	if(weight[i]<0) {
	  // if set to 1, this variable decreases the max by -weight[i]
	  if(bound_[1]+weight[i] < lower_bound) {
	    
#ifdef _DEBUG_IWEIGHTEDBOOLSUM
	    if(_DEBUG_IWEIGHTEDBOOLSUM) {
	      std::cout << scope[i] << " cannot be set to 1 because " 
			<< bound_[1] << " + " << weight[i] << " < " 
			<< lower_bound << std::endl;
	    }
#endif
	    
	    if(FAILED(scope[i].set_domain(0))) wiped_idx = FAILURE(i);
	    else bound_[0] -= weight[i];
	  }
	  // if set to 0, this variable increases the min by -weight[i]
	  else if(bound_[0]-weight[i] > upper_bound) {
	    
#ifdef _DEBUG_IWEIGHTEDBOOLSUM
	    if(_DEBUG_IWEIGHTEDBOOLSUM) {
	      std::cout << scope[i] << " cannot be set to 0 because " 
			<< bound_[1] << " - " << weight[i] << " > " 
			<< upper_bound << std::endl;
	    }
#endif
	    
	    if(FAILED(scope[i].set_domain(1))) wiped_idx = FAILURE(i);
	    else bound_[1] += weight[i];
	  }
	  else break;
	} else {
	  // if set to 1, this variable increases the min by weight[i]
	  if(bound_[0]+weight[i] > upper_bound) {
	    
#ifdef _DEBUG_IWEIGHTEDBOOLSUM
	    if(_DEBUG_IWEIGHTEDBOOLSUM) {
	      std::cout << scope[i] << " cannot be set to 1 because " 
			<< bound_[1] << " + " << weight[i] << " > " 
			<< upper_bound << std::endl;
	    }
#endif
	    
	    if(FAILED(scope[i].set_domain(0))) wiped_idx = FAILURE(i);
	    else bound_[1] -= weight[i];
	  }
	  // if set to 0, this variable decreases the max by weight[i]
	  else if(bound_[1]-weight[i] < lower_bound) {
	    
#ifdef _DEBUG_IWEIGHTEDBOOLSUM
	    if(_DEBUG_IWEIGHTEDBOOLSUM) {
	      std::cout << scope[i] << " cannot be set to 0 because " 
			<< bound_[1] << " - " << weight[i] << " < " 
			<< lower_bound << std::endl;
	    }
#endif
	    
	    if(FAILED(scope[i].set_domain(1))) wiped_idx = FAILURE(i);
	    else bound_[0] += weight[i];
	  }
	  else break;
	}
      }
 
     --i;

#ifdef _DEBUG_IWEIGHTEDBOOLSUM
      if(_DEBUG_IWEIGHTEDBOOLSUM) {
	std::cout << "\ncurrent bounds: [" << bound_[0] << ".." << bound_[1] << "]" << std::endl;
      }
#endif
      
    }
    index_ = i;


    if(bound_[0]>upper_bound || bound_[1]<lower_bound) {
      
#ifdef _DEBUG_CARD
      if(_DEBUG_CARD) {
	std::cout << "FAILURE!\n";
      }
#endif
      
    wiped_idx = FAILURE(changes[0]);
    }
  }


#ifdef _DEBUG_IWEIGHTEDBOOLSUM
  if(_DEBUG_IWEIGHTEDBOOLSUM) {
    int real_min = 0;
    int real_max = 0;
    for(i=0; i<weight.size; ++i) {
      if(scope[i].is_ground()) {
	if(scope[i].get_value()) {
	  real_min += weight[i];
	  real_max += weight[i];
	}
      } else if(weight[i] < 0) {
	real_min += weight[i];
      } else {
	real_max += weight[i];
      }
    }
    
    if(real_min != bound_[0]) {
      std::cout << "(2) mismatch on lower bound: real = " << real_min << ", incremental = " << bound_[0] << std::endl;
      exit(1);
    }
    
    if(real_max != bound_[1]) {
      std::cout << "(2) mismatch on upper bound: real = " << real_max << ", incremental = " << bound_[1] << std::endl;
      exit(1);
    }
  }
#endif



  
  return wiped_idx;
}

int Mistral::ConstraintIncrementalWeightedBoolSumInterval::check( const int* s ) const 
{
  int i=weight.size, t=0;
  while(i--) {
    t+=(weight[i]*s[i]);
  }
  return (t < lower_bound || t > upper_bound); 
}

std::ostream& Mistral::ConstraintIncrementalWeightedBoolSumInterval::display(std::ostream& os) const {
  if(lower_bound > -INFTY) 
    os << lower_bound << " <= " ;

  os << weight[0] << "*" << scope[0]/*.get_var()*/ ;

  for(unsigned int i=1; i<weight.size; ++i) 
    os << " + " << weight[i] << "*" << scope[i]/*.get_var()*/;
  
  if(upper_bound < INFTY) 
    os << " <= " << upper_bound;
 

  return os;
}






//#define _DEBUG_PWEIGHTEDBOOLSUM true


Mistral::PredicateWeightedBoolSum::PredicateWeightedBoolSum(Vector< Variable >& scp)
  : GlobalConstraint(scp) { 
  priority = 1;
  for(unsigned int i=1; i<scope.size; ++i) {
    weight.add(1);
  }
}

Mistral::PredicateWeightedBoolSum::PredicateWeightedBoolSum(Vector< Variable >& scp, 
							    Vector< int >& wgt)
  : GlobalConstraint(scp) { 
  priority = 1;
  for(unsigned int i=0; i<wgt.size; ++i) {
    weight.add(wgt[i]);
  }
}

Mistral::PredicateWeightedBoolSum::PredicateWeightedBoolSum(std::vector< Variable >& scp, 
							    std::vector< int >& wgt)
  : GlobalConstraint(scp) { 
  priority = 1;
  for(unsigned int i=0; i<wgt.size(); ++i) {
    weight.add(wgt[i]);
  }
}

void Mistral::PredicateWeightedBoolSum::initialise() {
  ConstraintImplementation::initialise();
  

  int ordering[weight.size];
  for(unsigned int i=0; i<weight.size; ++i) {
    ordering[i] = i;
  }

  sort_array = weight.stack_;
  qsort(ordering, weight.size, sizeof(int), increasing_weight);

  Vector<Variable> X;//(weight.size);
  Vector<int> w;//(weight.size);
  for(unsigned int i=0; i<weight.size; ++i) {
     X.add(scope[ordering[i]]);
     w.add(weight[ordering[i]]);
  }

  int _min_ = 0;
  int _max_ = 0;
  for(unsigned int i=0; i<weight.size; ++i) {
    scope[i] = X[i];
    weight[i] = w[i];

    trigger_on(_VALUE_, scope[i]);

    if(weight[i]<0) {
      _min_ += weight[i];
    } else {
      _max_ += weight[i];
    }
  }
  trigger_on(_RANGE_, scope[weight.size]);

  //min_index.initialise(get_solver(), 0);
  index_.initialise(get_solver(), weight.size-1);
  bound_[0].initialise(get_solver(), _min_);
  bound_[1].initialise(get_solver(), _max_);

  GlobalConstraint::initialise();


  domains = new BoolDomain[weight.size];
  for(unsigned int i=0; i<weight.size; ++i) {
    Variable var = scope[i].get_var();
    domains[i] = var.bool_domain;
  }

}

void Mistral::PredicateWeightedBoolSum::mark_domain() {
  for(int i=weight.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR|BITSET_VAR|RANGE_VAR);
}


Mistral::PredicateWeightedBoolSum::~PredicateWeightedBoolSum() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete weightedsum constraint" << std::endl;
#endif
  delete [] domains;
}



void Mistral::PredicateWeightedBoolSum::initialise_activity(double *lact, double *vact, double norm) {
  
  int n = weight.size;
  
  double avg_weight = (double)(bound_[1] - bound_[0] + 1)/(double)n;

  int real_max = std::min((int)(bound_[1]), scope[n].get_max());
  int real_min = std::max((int)(bound_[0]), scope[n].get_min());

  double incr_0 = 0;
  double incr_1 = 0;

    long double sol_0 = 0;
    long double sol_1 = 0;

  if(n < 500) {

    long double total_asgn = pow(2.0, n);
    
    int k;
    
    // std::cout << "arity: " << n << std::endl;
    
    // std::cout << "reachable [" << bound_[0] << " .. " << bound_[1] << "]\n"; 

    // std::cout << "bounds [" << scope[n].get_min() << " .. " << scope[n].get_max() << "]\n"; 

    // std::cout << "avg_weight: " << avg_weight << std::endl;

    // std::cout << "total_asgn: " << total_asgn << std::endl;

    // double total_sol = 0;
    //  k = (int)((double)(real_min - bound_[0])/avg_weight);
    // for(double val=real_min; val<=real_max; val+=avg_weight) {
    //   total_sol += bi_coeff(n, k);
    //   ++k;
    // }
  
    // std::cout << "total_sol: " << total_sol << std::endl;


    k = (int)((double)(real_min - bound_[0])/avg_weight);
    // int l = k;
    // int u = k;
    for(double val=real_min; val<=real_max; val+=avg_weight) {
      sol_0 += bi_coeff(n-1, k);
      // u = k;
      ++k;
    }

    // std::cout << "[" << l << "," << u << "]\n";
    // double sbc = sum_bi_coeff(n-1, l, u);

    // if(sbc != sol_0) {
    //   std::cout << sol_0 << " <3> " << sbc << std::endl;
    //   exit(1);
    // }

    // // case x = 0;
    // double sol_0 = 0;
    //  k = (int)((double)(real_min - bound_[0])/avg_weight);
    // for(double val=real_min; val<real_max; val+=avg_weight) {
    
    
    //   sol_0 += bi_coeff(n-1, k);

    //   //std::cout << "  -> " << k << " " << sol_0 << std::endl;


    //   ++k;
    // }

    //std::cout << "#sol[x=0]: " << sol_0 << std::endl;

    // case x = 1;

    k = (int)((double)(real_min - bound_[0])/avg_weight)-1;
    for(double val=real_min; val<real_max; val+=avg_weight) {

      sol_1 += bi_coeff(n-1, k);

      //std::cout << "  -> " << k << " " << sol_1 << std::endl;

      ++k;
    }

    //std::cout << "#sol[x=1]: " << sol_1 << std::endl;

    double ratio_0 = 2*(sol_0/total_asgn);
    double ratio_1 = 2*(sol_1/total_asgn);

    // std::cout << "ratio #sol[x=0]: " << ratio_0 << " weight: " << (1-ratio_0) << std::endl;

    // std::cout << "ratio #sol[x=1]: " << ratio_1 << " weight: " << (1-ratio_1) << std::endl;
  
    incr_0 = norm*(1-ratio_0);
    incr_1 = norm*(1-ratio_1);

    // std::cout << "ratio #sol[x=0]: " << ratio_0 << " weight: " << incr_0 << std::endl;

    // std::cout << "ratio #sol[x=1]: " << ratio_1 << " weight: " << incr_1 << std::endl;
  }


  if(incr_0 < 0.0001 && incr_1 < 0.0001) {
    incr_0 = 0.0001 * norm;
    incr_1 = 0.0001 * norm;
  }

  double total_weight = incr_1+incr_0;
  // 

  if(sol_0 != sol_1) {
    incr_1 += total_weight * (sol_0/(sol_0+sol_1));
    incr_0 += total_weight * (sol_1/(sol_0+sol_1));
    //total_weight *=2;  
  }


  //std::cout << "weights: " << incr_0 << " " << incr_1 << " " << avg_weight << std::endl;

	      
  int i=n, idx;
  while(i--) {
    idx = scope[i].id();

    //std::cout << vact[idx] << " " << lact[2*idx] << " " << lact[2*idx+1] << " => ";

    if(weight[i] > 0) {
      lact[2*idx+1] += incr_1 * (double)(weight[i]) / avg_weight;
      vact[idx] += incr_1 * (double)(weight[i]) / avg_weight;
    } else {
      lact[2*idx] += incr_0 * (double)(weight[i]) / avg_weight;
      vact[idx] += incr_0 * (double)(weight[i]) / avg_weight;
    }

    //std::cout << vact[idx] << " " << lact[2*idx] << " " << lact[2*idx+1] << std::endl;

    //lact[2*idx+1] += incr_1 * (double)(weight[i]) / avg_weight;
    //vact[idx] += (incr_0+incr_1) * (double)(weight[i]) / avg_weight;
  }

  // std::cout << "HERE" << std::endl;

  //    exit(1);


  /*
  // std::cerr << "initialise activity for " << this << std::endl;
  
  // std::cerr << lact << " " << vact << std::endl;

  int n = weight.size;
  int real_max = std::min((int)(bound_[1]), upper_bound);
  int real_min = std::max((int)(bound_[0]), lower_bound);

  //std::cerr << real_min << " .. " << real_max << std::endl;

  double span = (real_max - real_min);

  double center = (double)(real_min + real_max) / 2;
  double skew = center/(double)(bound_[1] - bound_[0] + 1);
  if(skew<0) skew = -skew;

  int log_n = 0, j=n;
  while(j) {
    j>>=1;
    ++log_n;
  }

  int exp = (int)(log_n * (span)/(double)(bound_[1] - bound_[0] + 1));


  std::cout << "exp: " << exp << std::endl;

  std::cout << "skew: " << skew << std::endl;

  double activity_increment = norm / (1 << exp);
  double ac_i; 

  std::cout << activity_increment << std::endl;


  int i=n;
  while(i--) {
    //std::cerr << i << std::endl;
    ac_i = activity_increment * (double)(std::abs(weight[i])) / (double)(std::abs(weight.back()));

    // std::cerr << 11111 << "  " << (scope[i]) << " " << (2*scope[i].id()+(weight[i]<0)) << std::endl ;

    // std::cerr << lact << " " << lact[(2*scope[i].id()+(weight[i]<0))] << std::endl;

    lact[2*scope[i].id()+(weight[i]<0)] += (1-skew) * ac_i;

    //std::cerr << 22222 << std::endl;

    lact[2*scope[i].id()+(weight[i]>=0)] += skew * ac_i;

    //std::cerr << 33333 << std::endl;

    vact[scope[i].id()] += ac_i;

    std::cout << scope[i] << " * " << weight[i] << ": " << ac_i << " (" << skew*ac_i << "/" << (1-skew)*ac_i << ")" << std::endl;
  }
  */
}


Mistral::Explanation::iterator Mistral::PredicateWeightedBoolSum::get_reason_for(const Atom a, const int lvl, Explanation::iterator& end) {
 
  explanation.clear();
  int i, idx;
  int *rank = get_solver()->assignment_order.stack_;
  int a_rank = INFTY-1;
  Explanation **reason = get_solver()->reason_for.stack_;


#ifdef _DEBUG_REASONIWBS
  std::cout << "compute reason for " << a << std::endl;
#endif


  // direction is 1 iff the constraint pushed toward ub and 0 otherwise
  bool direction = 1; 

  if(a != NULL_ATOM) {
    a_rank = rank[a];
    for(i=active.size; i<weight.size; ++i) {
      if(active[i] == a) {
	i = active[i];
	direction = (GET_VAL(domains[i]) == weight[i]>0);
      }
    }
  } else {
    direction = (bound_[1] < scope.back().get_min());
  }

  // finds all the assignment that decreased the maximum reachable value
      i = weight.size;
      while(i--) {
    idx = scope[i].id();
    if(idx != a && IS_GROUND(domains[i]) && rank[idx] < a_rank) {
      if((GET_VAL(domains[i]) == weight[i]>0) != direction) {
	explanation.add(NOT(literal(scope[i])));
      }
    }
  }

  end = explanation.end();
  return explanation.begin();
}


//#define _DEBUG_WEIGHTEDBOOLSUM (id == 102)

Mistral::PropagationOutcome Mistral::PredicateWeightedBoolSum::propagate() 
{
  
  PropagationOutcome wiped_idx = CONSISTENT;
  int w, i;


#ifdef _DEBUG_PWEIGHTEDBOOLSUM
  if(_DEBUG_PWEIGHTEDBOOLSUM) {
    std::cout << std::endl << "propagate " ;
    for(i=0; i<weight.size; ++i) {
      std::cout << " " << weight[i] << scope[i] << ":" << scope[i].get_domain();
    }
    std::cout << " = " << scope.back().get_domain() << std::endl << changes << std::endl;
  }
#endif


#ifdef _DEBUG_PWEIGHTEDBOOLSUM
    if(_DEBUG_PWEIGHTEDBOOLSUM) {
      std::cout << "\ncurrent bounds: [" << bound_[0] << ".." << bound_[1] << "]" << std::endl;
    }
#endif



  while(!changes.empty()) {
    i = changes.pop();

    if(i<weight.size) {

      w = weight[i]<0;
    

#ifdef _DEBUG_PWEIGHTEDBOOLSUM
      if(_DEBUG_PWEIGHTEDBOOLSUM) {
	std::cout << "\nprocessing events: " << event2str(event_type[i]) << " on " 
		  << scope[i] << " in " << scope[i].get_domain() << std::endl;
      }
#endif
      
      if(LB_CHANGED(event_type[i])) {
	bound_[w]+=weight[i];
      } else {
	bound_[1-w]-=weight[i];
      }    
    }


#ifdef _DEBUG_PWEIGHTEDBOOLSUM
    if(_DEBUG_PWEIGHTEDBOOLSUM) {
      std::cout << "\ncurrent bounds: [" << bound_[0] << ".." << bound_[1] << "]" << std::endl;
    }
#endif

  }


#ifdef _DEBUG_PWEIGHTEDBOOLSUM
  if(_DEBUG_PWEIGHTEDBOOLSUM) {
    int real_min = 0;
    int real_max = 0;
    for(i=0; i<weight.size; ++i) {
      if(scope[i].is_ground()) {
	if(scope[i].get_value()) {
	  real_min += weight[i];
	  real_max += weight[i];
	}
      } else if(weight[i] < 0) {
	real_min += weight[i];
      } else {
	real_max += weight[i];
      }
    }

    if(real_min != bound_[0]) {
      std::cout << "(1) mismatch on lower bound: real = " << real_min << ", incremental = " << bound_[0] << std::endl;
      exit(1);
    }

    if(real_max != bound_[1]) {
      std::cout << "(1) mismatch on upper bound: real = " << real_max << ", incremental = " << bound_[1] << std::endl;
      exit(1);
    }

    if(_DEBUG_PWEIGHTEDBOOLSUM) {
      std::cout << "\ntarget bounds: " << scope.back().get_domain() << std::endl;
    }
  }
#endif



  
   
  if(bound_[0]>scope.back().get_max() || bound_[1]<scope.back().get_min()) {
    
#ifdef _DEBUG_CARD
    if(_DEBUG_CARD) {
       std::cout << "FAILURE!\n";
    }
#endif
    
    wiped_idx = FAILURE(weight.size);
  } // else if(bound_[0] >= scope.back().get_min() && bound_[1] <= scope.back().get_max()) {
    
// #ifdef _DEBUG_CARD
//     if(_DEBUG_CARD) {
// 	std::cout << "ENTAILED!\n";
//       }
// #endif
    
//     relax();
//   } 
  else {

    scope.back().set_domain(bound_[0], bound_[1]);


    if(!scope.back().includes(bound_[0], bound_[1])) {


      i = index_;
      
      while(i>=0) {
	// 
	while(i>=0 && scope[i].is_ground()) --i;
	
	if(i>=0) {
	  
#ifdef _DEBUG_PWEIGHTEDBOOLSUM
	  if(_DEBUG_PWEIGHTEDBOOLSUM) {
	    std::cout << "\nnext index: "<< i << ": " << scope[i] << " in " << scope[i].get_domain() 
		      << " * " << weight[i] << std::endl;
	  }
#endif
	  
	  if(weight[i]<0) {
	    // if set to 1, this variable decreases the max by -weight[i]
	    if(bound_[1]+weight[i] < scope.back().get_min()) {
	      
#ifdef _DEBUG_PWEIGHTEDBOOLSUM
	      if(_DEBUG_PWEIGHTEDBOOLSUM) {
		std::cout << scope[i] << " cannot be set to 1 because " 
			  << bound_[1] << " + " << weight[i] << " < " 
			  << scope.back().get_domain() << std::endl;
	      }
#endif
	      
	    if(FAILED(scope[i].set_domain(0))) wiped_idx = FAILURE(i);
	    else bound_[0] -= weight[i];
	    }
	    // if set to 0, this variable increases the min by -weight[i]
	    else if(bound_[0]-weight[i] > scope.back().get_max()) {
	      
#ifdef _DEBUG_PWEIGHTEDBOOLSUM
	      if(_DEBUG_PWEIGHTEDBOOLSUM) {
		std::cout << scope[i] << " cannot be set to 0 because " 
			  << bound_[1] << " - " << weight[i] << " > " 
			  << scope.back().get_domain() << std::endl;
	      }
#endif
	      
	      if(FAILED(scope[i].set_domain(1))) wiped_idx = FAILURE(i);
	      else bound_[1] += weight[i];
	    }
	    else break;
	  } else {
	    // if set to 1, this variable increases the min by weight[i]
	    if(bound_[0]+weight[i] > scope.back().get_max()) {
	      
#ifdef _DEBUG_PWEIGHTEDBOOLSUM
	      if(_DEBUG_PWEIGHTEDBOOLSUM) {
		std::cout << scope[i] << " cannot be set to 1 because " 
			<< bound_[1] << " + " << weight[i] << " > " 
			  << scope.back().get_domain() << std::endl;
	      }
#endif
	      
	      if(FAILED(scope[i].set_domain(0))) wiped_idx = FAILURE(i);
	      else bound_[1] -= weight[i];
	  }
	    // if set to 0, this variable decreases the max by weight[i]
	    else if(bound_[1]-weight[i] < scope.back().get_min()) {
	      
#ifdef _DEBUG_PWEIGHTEDBOOLSUM
	      if(_DEBUG_PWEIGHTEDBOOLSUM) {
		std::cout << scope[i] << " cannot be set to 0 because " 
			  << bound_[1] << " - " << weight[i] << " < " 
			  << scope.back().get_domain() << std::endl;
	      }
#endif
	      
	      if(FAILED(scope[i].set_domain(1))) wiped_idx = FAILURE(i);
	      else bound_[0] += weight[i];
	    }
	  else break;
	  }
	}
 

	scope.back().set_domain(bound_[0], bound_[1]);
	--i;
	
#ifdef _DEBUG_PWEIGHTEDBOOLSUM
      if(_DEBUG_PWEIGHTEDBOOLSUM) {
	std::cout << "\ncurrent bounds: [" << bound_[0] << ".." << bound_[1] << "]" << std::endl;
      }
#endif
      
      }
      index_ = i;
    }
  }
    

#ifdef _DEBUG_PWEIGHTEDBOOLSUM
  if(_DEBUG_PWEIGHTEDBOOLSUM) {
    int real_min = 0;
    int real_max = 0;
    for(i=0; i<weight.size; ++i) {
      if(scope[i].is_ground()) {
	if(scope[i].get_value()) {
	  real_min += weight[i];
	  real_max += weight[i];
	}
      } else if(weight[i] < 0) {
	real_min += weight[i];
      } else {
	real_max += weight[i];
      }
    }
    
    if(real_min != bound_[0]) {
      std::cout << "(2) mismatch on lower bound: real = " << real_min << ", incremental = " << bound_[0] << std::endl;
      exit(1);
    }
    
    if(real_max != bound_[1]) {
      std::cout << "(2) mismatch on upper bound: real = " << real_max << ", incremental = " << bound_[1] << std::endl;
      exit(1);
    }
  }
#endif
  
  return wiped_idx;
}

int Mistral::PredicateWeightedBoolSum::check( const int* s ) const 
{
  int i=weight.size, t=0;
  while(i--) {
    t+=(weight[i]*s[i]);
  }

  //std::cout << t << " =? " << s[weight.size] << std::endl;

  return t != s[weight.size]; //(t < s[weight.size] || t > s[weight.size]);
}

std::ostream& Mistral::PredicateWeightedBoolSum::display(std::ostream& os) const {
  os << weight[0] << "*" << scope[0]/*.get_var()*/ ;

  for(unsigned int i=1; i<weight.size; ++i) 
    os << " + " << weight[i] << "*" << scope[i]/*.get_var()*/;
  
  os << " = " << scope[weight.size];
  
  return os;
}





Mistral::PredicateElement::PredicateElement(Vector< Variable >& scp, const int o)
  : GlobalConstraint(scp) {
  offset = o;
  priority = 1;
}

Mistral::PredicateElement::PredicateElement(std::vector< Variable >& scp, const int o)
  : GlobalConstraint(scp) { 
  offset = o;
  priority = 1;
}

void Mistral::PredicateElement::initialise() {
  ConstraintImplementation::initialise();

  int n = scope.size-1;
  aux_dom.initialise( std::min( 0, scope[n].get_min() ), 
		      std::max( n, scope[n].get_max() ), 
		      BitSet::empt );

  for(unsigned int i=0; i<scope.size; ++i)
    trigger_on(_DOMAIN_, scope[i]);
  //set_idempotent(true);

  GlobalConstraint::initialise();

  /////
  scope[n-1].set_min(0+offset);
  scope[n-1].set_max(n-2+offset);

}

void Mistral::PredicateElement::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}

Mistral::PredicateElement::~PredicateElement() 
{ 
#ifdef _DEBUG_MEMORY
  std::cout << "c delete element predicate" << std::endl;
#endif
}


Mistral::PropagationOutcome Mistral::PredicateElement::propagate() 
{

  PropagationOutcome wiped = CONSISTENT;
  int i, n = scope.size-2, evt, nxt; //, lb, ub, val;
  //Variable N = scope[n];
  //Variable V = scope[n+1];
  bool update_V = true;

#ifdef _DEBUG_ELEMENT 
  if(_DEBUG_ELEMENT) {
    std::cout << std::endl << std::endl << "X: " << scope[0].get_domain();
    for(i=1; i<n; ++i) {
      std::cout << " " << scope[i].get_domain();
    }
    std::cout << "[" << scope[n].get_domain() << "-" << offset << "] = " << scope[n+1].get_domain() << std::endl;
  } 
#endif

  while(IS_OK(wiped) && update_V) {
    update_V = false;
    while(IS_OK(wiped) && !changes.empty()) {
    
#ifdef _DEBUG_ELEMENT 
      if(_DEBUG_ELEMENT) {
	std::cout << changes << " " << events << std::endl;
      } 
#endif
      evt = changes.pop();
  
#ifdef _DEBUG_ELEMENT 
      if(_DEBUG_ELEMENT) {
	std::cout << "react to " << scope[evt] << " in " << scope[evt].get_domain() << std::endl;
      } 
#endif
      if(evt < n && scope[n].contain(evt+offset)) {
  
#ifdef _DEBUG_ELEMENT 
	if(_DEBUG_ELEMENT) {
	  std::cout << "  update " << scope[n] << " in " << scope[n].get_domain() << ": "
		    << scope[n+1] << " in " << scope[n+1].get_domain() << " inter "
		    << scope[evt] << " in " << scope[evt].get_domain() << "?" << std::endl;
	} 
#endif
	update_V = true;
	// scope[n] may change if scope[n+1] changes, or any X changes
	if( !scope[n+1].intersect(scope[evt]) ) {
	  
// 	  event_type[n] = scope[n].remove(evt+offset);
// 	  if( IS_FAIL(event_type[n]) ) {
// 	    wiped = FAILURE(n);
	    
// #ifdef _DEBUG_ELEMENT 
// 	    if(_DEBUG_ELEMENT) {
// 	      std::cout << "  => FAIL" << std::endl;
// 	    } 
// #endif	
// 	  } else if(!changes.contain(n)) {
// 	    changes.add(n);

// #ifdef _DEBUG_ELEMENT 
// 	    if(_DEBUG_ELEMENT) {
// 	      std::cout << "  => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
// 	    } 
// #endif
// 	  }

	  FILTER1( n, remove(evt+offset) );


	}
      } else if(evt == n) {
	update_V = true;
	if(ASSIGNED(event_type[n])) {
	  // X may change if scope[n] becomes assigned 
	  i = scope[n].get_min()-offset;
	  
#ifdef _DEBUG_ELEMENT 
	  if(_DEBUG_ELEMENT) {
	    std::cout << scope[n+1] << " in " << scope[n+1].get_domain() << " == "
		      << scope[i] << " in " << scope[i].get_domain() << std::endl;
	  } 
#endif
	  
	  event_type[i] = scope[i].set_domain(scope[n+1]);
	  if( IS_FAIL(event_type[i]) ) { 
#ifdef _DEBUG_ELEMENT 
	    if(_DEBUG_ELEMENT) {
	      std::cout << "  => FAIL" << std::endl;
	    } 
#endif
	    wiped = FAILURE(i);
	  } else {
	    if( event_type[i] != NO_EVENT && !changes.contain(i) ) {
	      changes.add(i);
#ifdef _DEBUG_ELEMENT 
	      if(_DEBUG_ELEMENT) {
		std::cout << "  => " << scope[i] << " in " << scope[i].get_domain() << std::endl;
	      } 
#endif
	    }
// 	    event_type[n+1] = scope[n+1].set_domain(scope[i]);
// 	    if( IS_FAIL(event_type[n+1]) ) {
// #ifdef _DEBUG_ELEMENT 
// 	      if(_DEBUG_ELEMENT) {
// 		std::cout << "  => FAIL" << std::endl;
// 	      } 
// #endif
// 	      wiped = FAILURE(n+1);
// 	    } else if( event_type[n+1] != NO_EVENT && !changes.contain(n+1) ) {
// 	      changes.add(n+1);
// #ifdef _DEBUG_ELEMENT 
// 	      if(_DEBUG_ELEMENT) {
// 		std::cout << "  => " << scope[n+1] << " in " << scope[n+1].get_domain() << std::endl;
// 	      } 
// #endif
// 	    }

	    FILTER1( n+1 , set_domain(scope[i]) );

	  }
	}
      } else if(evt == n+1) {
#ifdef _DEBUG_ELEMENT 
	if(_DEBUG_ELEMENT) {
	  std::cout << "  update " << scope[n] << " in " << scope[n].get_domain() << ": "
		    << scope[n+1] << " in " << scope[n+1].get_domain() << " inter " << std::endl;
	} 
#endif
	
	if( scope[n].is_ground() ) {
#ifdef _DEBUG_ELEMENT 
	  if(_DEBUG_ELEMENT) {
	    std::cout << "  update " << scope[n+1] << " in " << scope[n+1].get_domain() << std::endl;
	  } 
#endif
	  
	  i = scope[n].get_min()-offset;
// 	  event_type[i] = scope[i].set_domain(scope[n+1]);
// 	  if( IS_FAIL(event_type[i]) ) { 
// #ifdef _DEBUG_ELEMENT 
// 	    if(_DEBUG_ELEMENT) {
// 	      std::cout << "  => FAIL" << std::endl;
// 	    } 
// #endif
// 	    wiped = FAILURE(i);
// 	  } else {
// 	    if( event_type[i] != NO_EVENT && !changes.contain(i) ) {
// 	      changes.add(i);
// #ifdef _DEBUG_ELEMENT 
// 	      if(_DEBUG_ELEMENT) {
// 		std::cout << "  => " << scope[i] << " in " << scope[i].get_domain() << std::endl;
// 	      } 
// #endif
// 	    }
// 	  }

	  FILTER1( i, set_domain(scope[n+1]) );

	}

	// nxt = N.get_min();
	// do {
	//   i = nxt;
	  
	//   std::cout << " " << i ;
	  
	//   nxt = N.next(i);
	// } while( i<nxt );
	
	// std::cout << std::endl;

	  
	nxt = scope[n].get_min();
	do {
	  i = nxt;
	  nxt = scope[n].next(i);

#ifdef _DEBUG_ELEMENT 
	  if(_DEBUG_ELEMENT) {
	    std::cout << "  " << scope[i-offset] << " in " << scope[i-offset].get_domain() << "?" ;
	  } 
#endif
	  if( !scope[n+1].intersect(scope[i-offset]) ) {
#ifdef _DEBUG_ELEMENT 
	    if(_DEBUG_ELEMENT) {
	      std::cout << " NO" << std::endl;
	    } 
#endif	 

	    //std::cout << n << " " << N << " " << N.get_domain() << " " << i << std::endl;

// 	    event_type[n] = scope[n].remove(i);
// 	    if( IS_FAIL(event_type[n]) ) {
// #ifdef _DEBUG_ELEMENT 
// 	      if(_DEBUG_ELEMENT) {
// 		std::cout << "  => FAIL" << std::endl;
// 	      } 
// #endif
// 	      wiped = FAILURE(n);
// 	    } else if( event_type[n] != NO_EVENT && !changes.contain(n) ) {
// 	      changes.add(n);
// #ifdef _DEBUG_ELEMENT 
// 	      if(_DEBUG_ELEMENT) {
// 		std::cout << "  => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
// 	      } 
// #endif
// 	    }

	    FILTER1( n, remove(i) );
	    
#ifdef _DEBUG_ELEMENT 
	    if(_DEBUG_ELEMENT) {
	      if(event_type[n] == NO_EVENT) {
		std::cout << "  => NO EVENT" << std::endl;
	      }
	    } 
#endif	    
	  } 
#ifdef _DEBUG_ELEMENT 
	  
	  else if(_DEBUG_ELEMENT) {
	    std::cout << " YES" << std::endl;
	  }
#endif
	} while( i<nxt );
      }
    }
    
    if(IS_OK(wiped) && update_V) {
#ifdef _DEBUG_ELEMENT 
      if(_DEBUG_ELEMENT) {
	std::cout << "  update " << scope[n+1] << " in " << scope[n+1].get_domain() << std::endl;
      } 
#endif
      aux_dom.clear();
      //lb=INFTY; ub=-INFTY;
      
      // std::cout << "N " << n << ": " << scope[n] << " in " << scope[n].get_domain() << std::endl;
      // if(scope[n].domain_type == BITSET_VAR) {
      // 	scope[n].bitset_domain->debug_print();
      // }

      nxt = scope[n].get_min();
      do {
	i = nxt;
	// if(scope[i-offset].is_range()) {
	//   val = scope[i-offset].get_min();
	//   if(lb>val) lb = val;
	//   val = scope[i-offset].get_max();
	//   if(ub<val) ub = val;
	// } else {

	

	// std::cout << i << "-" << offset << " = " << (i-offset) << std::endl;

	// std::cout << scope << std::endl;


	scope[i-offset].union_to(aux_dom);
	//}
	nxt = scope[n].next(i);
#ifdef _DEBUG_ELEMENT 
	if(_DEBUG_ELEMENT) {
	  std::cout << " + " << scope[i-offset] << " in " << scope[i-offset].get_domain() << ": " << aux_dom << std::endl; 
	} 
#endif
      } while( i<nxt );
      // if(lb<=ub) aux_dom.fill(lb,ub);
      // std::cout << aux_dom << std::endl;
      
//       event_type[n+1] = scope[n+1].set_domain(aux_dom);
//       if( IS_FAIL(event_type[n+1]) ) { 
// #ifdef _DEBUG_ELEMENT 
// 	if(_DEBUG_ELEMENT) {
// 	  std::cout << "  => FAIL" << std::endl;
// 	} 
// #endif
// 	wiped = FAILURE(n+1);
//       } else if( event_type[n+1] != NO_EVENT && !changes.contain(n+1) ) {
// 	changes.add(n+1);
 #ifdef _DEBUG_ELEMENT 
 	if(_DEBUG_ELEMENT) {
 	  std::cout << "     " << scope[n+1] << " in " << scope[n+1].get_domain() << " = " << aux_dom << std::endl;
 	} 
 #endif
//       }

      FILTER1(n+1, set_domain(aux_dom) );

 #ifdef _DEBUG_ELEMENT 
 	if(_DEBUG_ELEMENT) {
 	  std::cout << "  => " << scope[n+1] << " in " << scope[n+1].get_domain() << std::endl;
 	} 
 #endif


    }
  }

#ifdef _DEBUG_ELEMENT 
  if(_DEBUG_ELEMENT) { 
    std::cout << "return " << wiped << std::endl;
    std::cout << "X: " << scope[0].get_domain();
    for(i=1; i<n; ++i) {
      std::cout << " " << scope[i].get_domain();
    }
    std::cout << "[" << scope[n].get_domain() << "] = " << scope[n+1].get_domain() << std::endl << std::endl ;
  } 
#endif 

  //exit(1);


  return wiped;
}

int Mistral::PredicateElement::check( const int* s ) const 
{
  return (s[s[scope.size-2]-offset] != s[scope.size-1]);
}

std::ostream& Mistral::PredicateElement::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/;
  for(unsigned int i=1; i<scope.size-2; ++i) {
    os << " " << scope[i]/*.get_var()*/;
  }
  os << ")[" << scope.back(2)/*.get_var()*/ << "] == " << scope.back()/*.get_var()*/;
  return os;
}


Mistral::ConstraintCliqueNotEqual::ConstraintCliqueNotEqual(Vector< Variable >& scp)
  : GlobalConstraint(scp) { priority = 2; }


void Mistral::ConstraintCliqueNotEqual::initialise() {
  ConstraintImplementation::initialise();
  for(unsigned int i=0; i<scope.size; ++i) {
    trigger_on(_VALUE_, scope[i]);
  }
  GlobalConstraint::initialise();
  //GlobalConstraint::set_idempotent(true);
}

void Mistral::ConstraintCliqueNotEqual::mark_domain() {
  for(unsigned int i=scope.size; i;) {
    //get_solver()->mark_non_convex(scope[i].id());
    get_solver()->forbid(scope[--i].id(), RANGE_VAR);
  }
}

Mistral::ConstraintCliqueNotEqual::~ConstraintCliqueNotEqual() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete cliquenotequal constraint" << std::endl;
#endif
}


//#define _DEBUG_CLIQUENOTEQUAL true

Mistral::PropagationOutcome Mistral::ConstraintCliqueNotEqual::propagate() 
{

  unsigned int i, j, n=active.size, m;
  int active_var, value;
  Event evt;

#ifdef _DEBUG_CLIQUENOTEQUAL
  std::cout << "propagate " << this << std::endl;
  for(i=0; i<scope.size; ++i) 
    std::cout << scope[i].get_domain() << (changes.contain(i) ? "* " : " " );
  std::cout << std::endl << active << std::endl;
#endif
  for(i=0; i<changes.size; ++i) {
    value = scope[changes[i]].get_min();

#ifdef _DEBUG_CLIQUENOTEQUAL
    std::cout << "react to " << scope[changes[i]] << "=" << value << std::endl; 
    std::cout << "  check pairs of recently assigned vars: " << std::endl;
#endif
    for(j=i+1; j<changes.size; ++j) {
#ifdef _DEBUG_CLIQUENOTEQUAL
      std::cout << "    " << scope[changes[j]] << "=" << scope[changes[j]].get_min() << " ";
#endif
      if(scope[changes[j]].get_min() == value) {
#ifdef _DEBUG_CLIQUENOTEQUAL
	std::cout << "FAIL!" << std::endl;
#endif
	return FAILURE(changes[j]);
      } 
#ifdef _DEBUG_CLIQUENOTEQUAL
      else std::cout << "OK!" << std::endl;
#endif
    }
    
#ifdef _DEBUG_CLIQUENOTEQUAL
    std::cout << "  remove this value from active vars: " << std::endl;
#endif
    // since the set of active variables might change while
    // processing this loop, we do it backward
    for(j=n; j; --j) {
      active_var = active[j-1];
      if(active_var != changes[i]) {

#ifdef _DEBUG_CLIQUENOTEQUAL
	std::cout << "    " << scope[active_var] << "-" << value << " ";
#endif
	
	evt = scope[active_var].remove(value);
	if(evt == FAIL_EVENT) {
#ifdef _DEBUG_CLIQUENOTEQUAL
	  std::cout << "FAIL!" << std::endl;
#endif
	  //assigned.clear();
	  return FAILURE(active_var);
	} else if(ASSIGNED(evt)) {
	  //assigned.add(active_var);
	  active.reversible_remove(active_var);
	}
#ifdef _DEBUG_CLIQUENOTEQUAL
	else std::cout << "OK!" << std::endl;
#endif
      }
    }
  }

  /// The following is to ensure idempotency
  m = active.size;

  //std::cout << n << " " << m << std::endl;

  while(m < n) {
    for(i=m; i<n; ++i) {
      value = scope[active[i]].get_min();
#ifdef _DEBUG_CLIQUENOTEQUAL
      std::cout << "rreact to " << scope[active[i]] << "=" << value << std::endl; 
      std::cout << "  check pairs of recently assigned vars: " << std::endl;
#endif
      for(j=i+1; j<n; ++j) {
#ifdef _DEBUG_CLIQUENOTEQUAL
	std::cout << "    " << scope[changes[j]] << "=" << scope[changes[j]].get_min() << " ";
#endif
	if(scope[active[j]].get_min() == value) {
#ifdef _DEBUG_CLIQUENOTEQUAL
	  std::cout << "FAIL!" << std::endl;
#endif
	  return FAILURE(active[j]);
	}
#ifdef _DEBUG_CLIQUENOTEQUAL
	else std::cout << "OK!" << std::endl;
#endif
      }

#ifdef _DEBUG_CLIQUENOTEQUAL
      std::cout << "  remove this value from active vars: " << std::endl;
#endif

      for(j=m; j; --j) {
	active_var = active[j-1];
#ifdef _DEBUG_CLIQUENOTEQUAL
	std::cout << "    " << scope[active_var] << "-" << value << " ";
#endif
	if(scope[active_var].remove(value) == FAIL_EVENT) {
#ifdef _DEBUG_CLIQUENOTEQUAL
	std::cout << "FAIL!" << std::endl;
#endif
	  return FAILURE(active_var);
	}
#ifdef _DEBUG_CLIQUENOTEQUAL
	else std::cout << "OK!" << std::endl;
#endif
      }
    }
    n = m;
    m = active.size;
  }

  return CONSISTENT;
}

int Mistral::ConstraintCliqueNotEqual::check( const int* s ) const 
{
  int i=scope.size, j;
  while(--i) {
    j=i;
    while(j--)
      if( s[i] == s[j] ) return 1;
  }
  return 0; 
}

std::ostream& Mistral::ConstraintCliqueNotEqual::display(std::ostream& os) const {
  os << "=/=(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << ", " << scope[i]/*.get_var()*/;
  os << ")" ; //<< events << " " << event_type;
  return os;
}


/**********************************************
 * Multi-AtMostSeqCard Constraint 
 **********************************************/

void Mistral::ConstraintMultiAtMostSeqCard::initialise_struct(const int k, const int d, const int* p, const int* q) {
  priority = 0;  

  _k = k;
  _d = d;
  if(_k) {
    _p = new int[_k];
    _q =  new int[_k];
    memcpy(_p, p, _k*sizeof(int));    
    memcpy(_q, q, _k*sizeof(int));
  }

  // std::cout << "initilialise structures of atmostseqcard(" << _k <<"):";
  // for(int i=0; i<_k; ++i) {
  //   std::cout << " " << _p[i] << "/" << _q[i] ;
  // }
  // std::cout << std::endl;

  
  wl = NULL; 
  wr = NULL; 
  occurrences = NULL; 
  cardinality = NULL; 
  lcumulated = NULL; 
  rcumulated = NULL; 
}

Mistral::ConstraintMultiAtMostSeqCard::ConstraintMultiAtMostSeqCard()
  : GlobalConstraint() { 
  initialise_struct();
}

Mistral::ConstraintMultiAtMostSeqCard::ConstraintMultiAtMostSeqCard(Vector< Variable >& scp, const int k, const int d, const int* p, const int* q)
  : GlobalConstraint(scp) { 
  initialise_struct(k, d, p, q);
}

Mistral::ConstraintMultiAtMostSeqCard::ConstraintMultiAtMostSeqCard(std::vector< Variable >& scp, const int k, const int d, const int* p, const int* q)
  : GlobalConstraint(scp) { 
  initialise_struct(k, d, p, q);
}

void Mistral::ConstraintMultiAtMostSeqCard::initialise() {
  ConstraintImplementation::initialise();
  for(unsigned int i=0; i<scope.size; ++i)
    trigger_on(_VALUE_, scope[i]);
  
  
  int max_q = _q[0];
  for(int i=1; i<_k; ++i) if(max_q < _q[i]) max_q = _q[i];
  
  int arity = scope.size;
  
  wl = new int[arity+2*max_q];
  std::fill(wl, wl+arity+2*max_q, 0);
  wl += max_q;
  
  wr = new int[arity+2*max_q];
  std::fill(wr, wr+arity+2*max_q, 0);
  wr += max_q;
  
  occurrences = new int*[_k];
  for(int i=0; i<_k; ++i) {
    occurrences[i] = new int[arity+_p[i]+1];
    std::fill(occurrences[i], occurrences[i]+arity+_p[i]+1, 0);
    occurrences[i] += arity;
  }
  
  cardinality = new int*[_k];
  for(int i=0; i<_k; ++i) {
    cardinality[i] = new int[2*_q[i]];
    std::fill(cardinality[i], cardinality[i]+2*_q[i], 0);
    cardinality[i] += _q[i];
  }


  lcumulated = new int[arity+1];
  rcumulated = new int[arity+1];
  reverse.initialise(0, arity);
  for(int i=0; i<arity; ++i) reverse.add(scope[arity-1-i]);
  
  GlobalConstraint::initialise();
}

void Mistral::ConstraintMultiAtMostSeqCard::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}

Mistral::ConstraintMultiAtMostSeqCard::~ConstraintMultiAtMostSeqCard() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete atmostseqcard constraint" << std::endl;
#endif

  int max_q = _q[0], arity=scope.size;
  for(int i=1; i<_k; ++i) if(max_q < _q[i]) max_q = _q[i];
  
  wl -= max_q;
  delete [] wl;
  
  wr -= max_q;
  delete [] wr;
  
  for(int i=1; i<_k; ++i) {
    occurrences[i] -= arity;
    delete [] occurrences[i];
  }
  delete [] occurrences;
  
  for(int i=1; i<_k; ++i) {
    cardinality[i] -= _q[i];
    delete [] cardinality[i];
  }
  delete [] cardinality;
  
  delete [] lcumulated;
  delete [] rcumulated;
  
  delete [] _q;
  delete [] _p;
}


bool Mistral::ConstraintMultiAtMostSeqCard::greedy_assign(int *w, int *cumulated, Vector<Variable>& X) {
  
  int arity = X.size;
  int i, j, k;

  int max_cardinality[_k];
  int count = 0;

  int o_c[_k];
  int n_c[_k];

#ifdef _DEBUG_AMSC
  int _max_card[_k*arity];
  int _count[_k*arity];

  int max_p = _p[0];
  for(k=1; k<_k; ++k) if(max_p < _p[k]) max_p = _p[k];
  int **_occ = new int*[_k];
  int **_card = new int*[_k];


  for(k=0; k<_k; ++k) {
    _occ[k] = new int[(arity*(arity+_p[k]+1))];
    std::fill(_occ[k], _occ[k]+(arity*(arity+_p[k]+1)), 0);
    _card[k] = new int[(arity*_q[k])];
  }
#endif

  bool maybe_inconsistent = true;

  for(i=0; i<arity; ++i) {
    count += w[i];
    cumulated[i] = 0;
    for(k=0; k<_k; ++k) {
      occurrences[k][i-arity] = 0;
    }
  }
 
  for(k=0; k<_k; ++k) {
    max_cardinality[k] = 0;
    for(i=0; i<=_p[k] && i<arity; ++i) { 
      occurrences[k][i] = 0; 
    }

    for(i=0; i<_q[k] && i<arity; ++i) {
      cardinality[k][i] = cardinality[k][i-1]+w[i];
      ++(occurrences[k][cardinality[k][i]]);
      if(cardinality[k][i] > max_cardinality[k]) max_cardinality[k] = cardinality[k][i];
    }
  }

  for(i=0; i<arity; ++i) {
    
    bool card_ok = true;
    for(k=0; card_ok && k<_k; ++k) {
      card_ok = (max_cardinality[k] < _p[k]);
    }
    if(card_ok && !X[i].is_ground()) {
      for(k=0; k<_k; ++k) ++max_cardinality[k];
      cumulated[i+1] = cumulated[i]+1;
      w[i] = 1;
      if(cumulated[i+1]+count > _d) {
	maybe_inconsistent = false;
	break;
      }
    } else {
      cumulated[i+1] = cumulated[i];
    }

#ifdef _DEBUG_AMSC
    if(_DEBUG_AMSC) {
    _count[i] = count;
    for(k=0; k<_k; ++k) {
      _max_card[i*_k+k] = max_cardinality[k];
      for(j=0; j<_q[k]; ++j) _card[k][(j*arity+i)] = cardinality[k][(i+j)%_q[k]]+cumulated[i+1];
      for(j=0; j<=arity+_p[k]; ++j) {
	
	// if(occurrences[k][j-arity] > 10) {
	//   cout << "_occ[" << k << "][" << j << "][" << i << "] = " 
	//        << "occurrences[" << k << "][" << j-arity << "] = " 
	//        << occurrences[k][j-arity] << endl;
	// }

	_occ[k][(j*arity+i)] = occurrences[k][j-arity];
      }
    }
    }
#endif

    for(k=0; k<_k; ++k) {
      o_c[k] = cardinality[k][i%_q[k]];
      n_c[k] = cardinality[k][(i+_q[k]-1)%_q[k]]+w[i+_q[k]]-w[i];
      cardinality[k][i%_q[k]] = n_c[k];
      
      if(o_c[k] != n_c[k]) {

	if(n_c[k] > _p[k]) {
	  std::cerr << id << " k=" << k << " / 1" << std::endl
		    << id << " n_c[k]=" << n_c[k] << " / [" << (-arity) << "," << _p[k] << "]" << std::endl << std::endl;
	}

	++occurrences[k][n_c[k]];
	if((n_c[k]+cumulated[i+1])>max_cardinality[k]) ++max_cardinality[k];
	if(!(--occurrences[k][o_c[k]]) && (o_c[k]+cumulated[i+1])==max_cardinality[k]) --max_cardinality[k];
      }
    }
    
  }


#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
    if(maybe_inconsistent) {
      std::cerr << std::endl << "propag " << _d-count  ;
      for(k=0; k<_k; ++k)
	std::cerr << " " << _p[k] << "/" << _q[k] ;
      std::cerr << std::endl;
      for(i=0; i<arity; ++i) {
	if(scope[i].is_ground()) std::cerr << scope[i].get_min();
      else std::cerr << ".";
      }
      std::cerr << std::endl;
      for(i=0; i<arity; ++i) {
	std::cerr << w[i];
      }
      std::cerr << std::endl;
      for(k=0; k<_k; ++k) {
	std::cerr << std::endl;
	for(i=0; i<arity; ++i) {
	  std::cerr << _max_card[_k*i+k];
	}
	std::cerr << std::endl;
	std::cerr << std::endl;
	for(j=0; j<_q[k]; ++j) {
	  for(i=0; i<arity; ++i) {
	    std::cerr << _card[k][(j*arity+i)];
	  }
	  std::cerr << std::endl;
	}
	std::cerr << std::endl;
	for(j=arity+_p[k]; j; --j) {
	  int tmax = 0;
	  for(i=0; i<arity; ++i) {
	    if(cumulated[i+1]+j+1 == arity)
	      std::cerr << "-";
	    else
	      std::cerr << _occ[k][(j*arity+i)];
	    tmax = (cumulated[i+1]+j+1 > arity);
	  }
	  std::cerr << std::endl;
	  if(!tmax) break;
	}
	std::cerr << std::endl;
      }
    }
  }
  
  for(k=0; k<_k; ++k) {
    delete [] _card[k];
    delete [] _occ[k];
  }
  delete [] _card;
  delete [] _occ;
  
  //exit(1);
  //}
#endif
  
  return maybe_inconsistent;
}


Mistral::PropagationOutcome Mistral::ConstraintMultiAtMostSeqCard::propagate() 
{
  PropagationOutcome wiped = CONSISTENT;
  int arity = scope.size;

  int count = 0;
  for(int i=0; i<arity; ++i) {
    wl[i] = scope[i].get_min();
    wr[arity-i-1] = wl[i];
    count += wl[i];
  }


#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
    int k;
    std::cerr << std::endl << "propag (" << id << ") " << _d-count ;
    for(k=0; k<_k; ++k)
      std::cerr << " " << _p[k] << "/" << _q[k] ;
    std::cerr << std::endl;
    for(int i=0; i<arity; ++i) {
      if(scope[i].is_ground()) std::cerr << scope[i].get_min();
      else std::cerr << ".";
    }
    std::cerr << std::endl ;//<< std::endl;
  }
#endif



  if(count < _d && greedy_assign(wl, lcumulated, scope)) {

    if(lcumulated[arity]+count < _d) {

#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
    std::cerr << "cannot reach the demand!!" << std::endl;
  }
#endif

      wiped = FAILURE(0);
    } else {

      greedy_assign(wr, rcumulated, reverse);

#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
      int k;
      std::cerr << std::endl << "propag " << _d-count ;
      for(k=0; k<_k; ++k)
	std::cerr << " " << _p[k] << "/" << _q[k] ;
      std::cerr << std::endl;
      for(int i=0; i<arity; ++i) {
	if(scope[i].is_ground()) std::cerr << scope[i].get_min();
	else std::cerr << ".";
      }
      std::cerr << std::endl ;//<< std::endl;
      
      for(int i=0; i<arity; ++i) {
	std::cerr << wl[i] ;
      }
      std::cerr << std::endl;
      for(int i=0; i<arity; ++i) {
	std::cerr << (lcumulated[i] % 10) ;
      }
      std::cerr << std::endl << std::endl;
      
      
      for(int i=0; i<arity; ++i) {
	if(scope[i].is_ground()) std::cerr << scope[i].get_min();
	else std::cerr << ".";
      }
      std::cerr << std::endl ;
      for(int i=arity; i--;) {
	std::cerr << wr[i] ;
      }
      std::cerr << std::endl;
      for(int i=arity; i--;) {
	std::cerr << (rcumulated[i] % 10) ;
      }
      std::cerr << std::endl << std::endl;
  }
      bool pruning = false;
#endif
      
      for(int i=0; // IS_OK(wiped) &&
	    i<arity; ++i)  {
	if(!scope[i].is_ground() && lcumulated[i] + rcumulated[arity-i-1] + count < _d) {
	  
#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
	  std::cerr << "set " << scope[i] << " to 1" << std::endl;
  }
#endif
	  
	  //if(FAILED(scope[i].set_domain(1))) wiped = FAILURE(i);
	  scope[i].set_domain(1);
	  
#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
	  pruning = true;
  }
#endif
	}
	
	if(!scope[i].is_ground() && lcumulated[i+1] + rcumulated[arity-i] + count <= _d) {

#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
	  std::cerr << "set " << scope[i] << " to 0" << std::endl;
  }
#endif
	  
	  //if(FAILED(scope[i].set_domain(0))) wiped = FAILURE(i);
	  scope[i].set_domain(0);
	  
#ifdef _DEBUG_AMSC
  if(_DEBUG_AMSC) {
	  pruning = true;
  }
#endif
	}
      }
      
#ifdef _DEBUG_AMSC
      if(_DEBUG_AMSC) {
      if(pruning) {
	std::cerr << "pruning " << _d-count << " " << _p << "/" << _q << std::endl;
	for(int i=0; i<arity; ++i) {
	  if(scope[i].is_ground()) std::cerr << scope[i].get_min();
	  else std::cerr << ".";
	}
	std::cerr << std::endl ;//<< std::endl;
      }
      }
#endif
      
    }
  }

  return wiped;
}

int Mistral::ConstraintMultiAtMostSeqCard::check( const int* s ) const 
{
  bool ok = true;
  int i, j, subseq_card[_k], total_card = 0, arity = scope.size;
  for(j=0; j<_k; ++j) 
    subseq_card[j] = 0;


  for(i=0; i<arity && ok; ++i) {
    total_card += s[i];

    // std::cout << std::setw(3) << total_card << std::setw(2) << s[i];

    if(total_card > _d) ok = false;
    for(j=0; j<_k && ok; ++j) {

      // std::cout << "[k=" << std::setw(1) << (j+1) << ": + " << s[i] ; 

      subseq_card[j] += s[i];
      if(i - _q[j] >= 0) {
	// std::cout << " - s[" << (i - _q[j]) << "]=" ;
	// std::cout.flush();
	// std::cout << s[i - _q[j]];
	subseq_card[j] -= s[i - _q[j]];
      }
      if(subseq_card[j] > _p[j]) ok = false;

      // std::cout << "] ";
    }
    //    std::cout << std::endl;
  }
  if(total_card < _d) ok = false;

  return !ok;
}




// Mistral::Explanation::iterator Mistral::ConstraintMultiAtMostSeqCard::get_reason_for(const Atom a, const int lvl, Explanation::iterator& end) {
//   explanation.clear();
//   int i=scope.size;

//   if(a != NULL_ATOM) {
//     if(get_solver()->variables[a].get_value()) {
//       // the literal we try to explain is positive
//       while(i--) {
// 	if(!(scope[i].get_max())) explanation.add(literal(scope[i]));
//       }
//     } else {
//       // the literal we try to explain is positive
//       while(i--) {
// 	if(  scope[i].get_min() ) explanation.add(literal(scope[i]));
//       }
//     }
//   } else {
//     if(gap > 0) {
//       // too many ones
//       while(i-- && explanation.size <= total) {
// 	if(scope[i].get_min()) explanation.add(literal(scope[i]));
//       }
//     } else {
//       // too many zeros
//       while(i-- && explanation.size <= scope.size-total) {
// 	if(!(scope[i].get_max())) explanation.add(literal(scope[i]));
//       }
//     }
//   }
//   end = explanation.end();
//   return explanation.begin();
// }


std::ostream& Mistral::ConstraintMultiAtMostSeqCard::display(std::ostream& os) const {
  os << "(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << " + " << scope[i]/*.get_var()*/;
  os << ") == " << _d  << " & AtMost(" << _p[0] << "/" << _q[0] ;
  for(unsigned int i=1; i<_k; ++i) 
    os << ", " << _p[i] << "/" << _q[i] ;
  os << ")";
  return os;
}




/**********************************************
 * AllDiff Constraint 
 **********************************************/


const int INCONSISTENT = 0;
const int CHANGES      = 1;
const int NO_CHANGES   = 2;

Mistral::ConstraintAllDiff::ConstraintAllDiff(Vector< Variable >& scp)
  : GlobalConstraint(scp) { priority = 0; }

Mistral::ConstraintAllDiff::ConstraintAllDiff(std::vector< Variable >& scp)
  : GlobalConstraint(scp) { priority = 0; }


void Mistral::ConstraintAllDiff::initialise() {

  ConstraintImplementation::initialise();

  for(unsigned int i=0; i<scope.size; ++i) {
    trigger_on(_RANGE_, scope[i]);
  }

  GlobalConstraint::initialise();

  //GlobalConstraint::set_idempotent(true);
  unsigned int i;
  //level = &(scope[0].get_solver()->level);
  lastLevel = -2;
  nb = 0;

  iv        = new AD_Interval[scope.size];
  minsorted = new AD_Interval*[scope.size];
  maxsorted = new AD_Interval*[scope.size];
  bounds    = new int[2*scope.size+2];
  std::fill(bounds, bounds+2*scope.size+2, 0);

  for( i=0; i<scope.size; ++i ) {
    minsorted[i] = maxsorted[i] = &iv[i];  
    iv[i].min = iv[i].max = NOVAL;
  }

  t = new int[2*scope.size+2];
  d = new int[2*scope.size+2];
  h = new int[2*scope.size+2];
}

void Mistral::ConstraintAllDiff::mark_domain() {
  for(unsigned int i=0; i<scope.size; ++i) {
    get_solver()->forbid(scope[i].id(), RANGE_VAR|LIST_VAR);
    //get_solver()->mark_non_convex(scope[i].id());
  }
}

Mistral::ConstraintAllDiff::~ConstraintAllDiff() 
{
#ifdef _DEBUG_MEMORY
  std::cout << "c delete alldiff constraint" << std::endl;
#endif
  delete [] bounds;
  delete [] maxsorted;
  delete [] minsorted;
  delete [] iv;
  delete [] h;
  delete [] d;
  delete [] t;
}

void sortmin( Mistral::AD_Interval *v[], int n ) 
{
  int i, current;
  bool sorted;
  Mistral::AD_Interval *t;

  current = n-1;
  sorted = false;
  while( !sorted ) {
    sorted = true;
    for( i = 0; i < current; i++ ) {
      if( v[i]->min > v[i+1]->min ) {
        t = v[i];
        v[i] = v[i+1];
        v[i+1] = t;
        sorted = false;
      }
    }
    current--;
  }
}

void sortmax( Mistral::AD_Interval *v[], int n ) 
{
  int i, current;
  bool sorted;
  Mistral::AD_Interval *t;

  current = 0;
  sorted = false;
  while( !sorted ) {
    sorted = true;
    for( i = n-1; i > current; i-- ) {
      if( v[i]->max < v[i-1]->max ) {
        t = v[i];
        v[i] = v[i-1];
        v[i-1] = t;
        sorted = false;
      }
    }
    current++;
  }
}

void Mistral::ConstraintAllDiff::sortit() 
{  
  int i,j,nb;
  int min,max,last;

  sortmin(minsorted, scope.size);
  sortmax(maxsorted, scope.size);

  min = minsorted[0]->min;
  max = maxsorted[0]->max + 1;
  bounds[0] = last = min-2;

  for (i=j=nb=0;;) { // merge minsorted[] and maxsorted[] into bounds[]
    if (i<(int)(scope.size) && min<=max) {	// make sure minsorted exhausted first
      if (min != last)
        bounds[++nb] = last = min;
      minsorted[i]->minrank = nb;
      if (++i < (int)(scope.size))
        min = minsorted[i]->min;
    } else {
      if (max != last)
	bounds[++nb] = last = max;
      maxsorted[j]->maxrank = nb;
      if (++j == (int)(scope.size)) break;
      max = maxsorted[j]->max + 1;
    }
  }
  ConstraintAllDiff::nb = nb;
  bounds[nb+1] = bounds[nb] + 2;
}


void pathset(int *t, int start, int end, int to) 
{
  int k, l;
  for (l=start; (k=l) != end; t[k]=to) 
    l = t[k];  
}

int pathmin(int *t, int i) 
{
  for (; t[i] < i; i=t[i]) ;
  return i;
}

int pathmax(int *t, int i) 
{
  for (; t[i] > i; i=t[i]) ;  
  return i;
}


int Mistral::ConstraintAllDiff::filterlower() 
{
  int i,j;
  int w,x,y,z,_changes_ = 0;

  for (i=1; i<=nb+1; i++)
    d[i] = bounds[i] - bounds[t[i]=h[i]=i-1];
  for (i=0; i<(int)(scope.size); i++) { // visit Intervals in increasing max order
    x = maxsorted[i]->minrank; y = maxsorted[i]->maxrank;
    j = t[z = pathmax(t, x+1)];
    if (--d[z] == 0)
      t[z = pathmax(t, t[z]=z+1)] = j;
    pathset(t, x+1, z, z); // path compression
    if (d[z] < bounds[z]-bounds[y]) return INCONSISTENT; // no solution
    if (h[x] > x) {
      maxsorted[i]->min = bounds[w = pathmax(h, h[x])];
      pathset(h, x, w, w); // path compression
      _changes_ = 1;
    }
    if (d[z] == bounds[z]-bounds[y]) {
      pathset(h, h[y], j-1, y); // mark hall Interval
      h[y] = j-1; //("hall Interval [%d,%d)\n",bounds[j],bounds[y]);
    }
  }
  if( _changes_ )
    return CHANGES;
  else
    return NO_CHANGES;
}


int Mistral::ConstraintAllDiff::filterupper()
{
  int i,j;
  int w,x,y,z,_changes_ = 0;

  for (i=0; i<=nb; i++)
    d[i] = bounds[t[i]=h[i]=i+1] - bounds[i];
  for (i=scope.size; --i>=0; ) { // visit Intervals in decreasing min order
    x = minsorted[i]->maxrank; y = minsorted[i]->minrank;
    j = t[z = pathmin(t, x-1)];
    if (--d[z] == 0)
      t[z = pathmin(t, t[z]=z-1)] = j;
    pathset(t, x-1, z, z);
    if (d[z] < bounds[y]-bounds[z]) return INCONSISTENT; // no solution
    if (h[x] < x) {
      minsorted[i]->max = bounds[w = pathmin(h, h[x])] - 1;
      pathset(h, x, w, w);
      _changes_ = 1;
    }
    if (d[z] == bounds[y]-bounds[z]) {
      pathset(h, h[y], j+1, y);
      h[y] = j+1;
    }
  }
  if( _changes_ )
    return CHANGES;
  else
    return NO_CHANGES;
}

Mistral::PropagationOutcome Mistral::ConstraintAllDiff::propagate() 
{
  unsigned int i, a, b;

  int status_lower, status_upper;
  int l, u;

   a = 0;
   b = scope.size;

  //if( lastLevel != ((solver->level) - 1) ) {
  if( lastLevel != ((solver->level) - 1) ) {
    // not incremental
    status_lower = CHANGES;
    status_upper = CHANGES;
    i = 0;
    while (i < scope.size) {
      iv[i].min = scope[i].get_min();
      iv[i].max = scope[i].get_max();
      i++;
    }
  }
  else {
    // incremental
    status_lower = NO_CHANGES;
    status_upper = NO_CHANGES;
    for( i = a; i < b; i++ ) {
      l = iv[i].min;
      u = iv[i].max;
      iv[i].min = scope[i].get_min();
      iv[i].max = scope[i].get_max();
      if( l != iv[i].min ) status_lower = CHANGES;
      if( u != iv[i].max ) status_upper = CHANGES;
    }
  }

//   //a = 0;
//   //b = scope.size;

//   //if( lastLevel != ((solver->level) - 1) ) {
//   if( lastLevel != ((*level) - 1) ) {
//     // not incremental
//     status_lower = CHANGES;
//     status_upper = CHANGES;
//     i = 0;
//     while (i < scope.size) {
//       iv[i].min = scope[i].get_min();
//       iv[i].max = scope[i].get_max();
//       i++;
//     }
//   }
//   else {
//     // incremental
//     status_lower = NO_CHANGES;
//     status_upper = NO_CHANGES;
//     //for( i = a; i < b; i++ ) {

//     //std::cout << "ccc " ;
//     for(unsigned int j=0; j<changes.size; ++j) {
//       i = changes[j];
// //       std::cout << i << " ";
// //     }
// //     std::cout << std::endl;

// //     std::cout << "rrr " ;
// //     for( i = a; i < b; i++ ) {

//       l = iv[i].min;
//       u = iv[i].max;
//       iv[i].min = scope[i].get_min();
//       iv[i].max = scope[i].get_max();

//       //      if( l != iv[i].min || u != iv[i].max ) std::cout << i << " ";

//       if( l != iv[i].min ) status_lower = CHANGES;
//       if( u != iv[i].max ) status_upper = CHANGES;
//     }
//     //    std::cout << std::endl << std::endl;
//   }

  //lastLevel = *level;//(solver->level);
  lastLevel = (solver->level);

  if( status_lower == NO_CHANGES && status_upper == NO_CHANGES ) 
    return CONSISTENT;

  sortit();



  status_lower = filterlower();
  if( status_lower != INCONSISTENT )
    status_upper = filterupper();  

  if( (status_lower == INCONSISTENT) || (status_upper == INCONSISTENT) ) 
    { return FAILURE(changes.back()); }
  else
    if( (status_lower == CHANGES) || (status_upper == CHANGES) ) {
      i = 0;
      while (i < scope.size) {
	if( scope[i].set_min( iv[i].min ) == FAIL_EVENT )  { return FAILURE(i); }
	if( scope[i].set_max( iv[i].max ) == FAIL_EVENT )  { return FAILURE(i); }
	i++;
      }
    }  
  return CONSISTENT;
}

int Mistral::ConstraintAllDiff::check( const int* s ) const 
{
  int i=scope.size, j;
  while(--i) {
    j=i;
    while(j--)
      if( s[i] == s[j] ) return 1;
  }
  return 0; 
}

std::ostream& Mistral::ConstraintAllDiff::display(std::ostream& os) const {
  os << "alldiff(" << scope[0]/*.get_var()*/ ;
  for(unsigned int i=1; i<scope.size; ++i) 
    os << ", " << scope[i]/*.get_var()*/;
  os << ")" ;
  return os;
}



Mistral::PredicateMin::PredicateMin(Vector< Variable >& scp) : GlobalConstraint(scp) { priority = 1; }

Mistral::PredicateMin::~PredicateMin() {
#ifdef _DEBUG_MEMORY
  std::cout << "c delete min predicate" << std::endl;
#endif
}

void Mistral::PredicateMin::initialise() {
  ConstraintImplementation::initialise();
  last_min = 0;
  for(unsigned int i=0; i<scope.size; ++i) {
    trigger_on(_RANGE_, scope[i]);
    if(scope[i].get_min() < scope[last_min].get_min())
      last_min = i;
  }
  GlobalConstraint::initialise();

  candidates.initialise(solver, 0, scope.size-2, scope.size-1, true);
  int n = scope.size-1;
  for(int i=0; i<n; ++i) {
    if(scope[i].get_min() > scope[n].get_max()) {
      candidates.reversible_remove(i);
      //relax_from(i);
      if(!scope[i].is_ground()) un_post_from(i);
      //un_post_from(i);
    }
  }

  enforce_nfc1 = false;
  //set_idempotent(true);
}

void Mistral::PredicateMin::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}


int Mistral::PredicateMin::check( const int* sol ) const {
  int i=scope.size-1, the_min=sol[0], smin = sol[i];
  while( --i > 0 ) {
    if(sol[i] < the_min) the_min = sol[i];
    if(the_min < smin) break;
  }
  return the_min != smin;
}


void Mistral::PredicateMin::react_to(PropagationOutcome& wiped, const int changed_idx, const Event evt) {
  //PropagationOutcome wiped = CONSISTENT;
  int n = scope.size-1, i, var, ub, lb;
  int cs = candidates.size;

#ifdef _DEBUG_MIN
  std::cout << "react to " << event2str(evt) << " on " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << std::endl;
#endif
  
  if(changed_idx<n) {
    event_type[n] = 0;
    if(candidates.contain(changed_idx)) {
#ifdef _DEBUG_MIN
      std::cout << "  " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << " is a candidate" << std::endl;
#endif
      if(candidates.size == 1) { // equality
#ifdef _DEBUG_MIN
	std::cout << "    => the only one! (enforce equality)" << std::endl;
#endif
	FILTER3(n, set_domain(scope[changed_idx]));
#ifdef _DEBUG_MIN
	if(IS_OK(wiped)) {
	  std::cout << "    => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
	} else {
	  std::cout << "    => fail!" << std::endl;
	}
#endif
      } else {// scope[i] >= scope[n]
	if(UB_CHANGED(evt)) {
#ifdef _DEBUG_MIN
	  std::cout << "    => update " << scope[n] << "'s ub" << std::endl;
#endif

	  //std::cout << wiped << std::endl;

	  FILTER3(n, set_max(scope[changed_idx].get_max()));

	  // std::cout << scope[n] << " in " << scope[n].get_domain() << " <= "
	  // 	    << scope[changed_idx].get_max() << " " << wiped << std::endl;
	
#ifdef _DEBUG_MIN
	  if(IS_OK(wiped)) {
	    std::cout << "    => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
	  } else {
	    std::cout << "    => fail!" << std::endl;
	  }
#endif
	} 
	if(LB_CHANGED(evt)) {
	  
	  if(scope[n].get_max() < scope[changed_idx].get_min()) { 
#ifdef _DEBUG_MIN
	    std::cout << "    => update " << scope[changed_idx] << "'s possibility" << std::endl;
#endif
	    // scope[i] may not be the minimum variable anymore?
	    candidates.reversible_remove(changed_idx);
#ifdef _DEBUG_MIN
	    std::cout << "    => " << candidates << std::endl;
#endif
	  } 
	  if(scope[n].get_min() < scope[changed_idx].get_min()) {
#ifdef _DEBUG_MIN
	    std::cout << "    => " << scope[changed_idx] << " was (maybe) the witness for " 
		      << scope[n] << "'s min" << std::endl;
#endif
	    lb = INFTY;
	    for(i=candidates.size; i;) {
	      --i;
	      var = candidates[i];
	      if(scope[var].get_min() < lb) {
		lb = scope[var].get_min();
	      }
	    }
#ifdef _DEBUG_MIN
	    std::cout << "      => new lb = " << lb << std::endl;
#endif
	    FILTER3(n, set_min(lb));
	  }
	}
      }
    }
#ifdef _DEBUG_MIN
    else {
      std::cout << "  " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << " is not a candidate" << std::endl;
    }
#endif
  } else {
#ifdef _DEBUG_MIN
    std::cout << "  " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << " is the MIN" << std::endl;
#endif
    if(candidates.size == 1) { // equality
#ifdef _DEBUG_MIN
      std::cout << "    => only one candidate (enforce equality)" << std::endl;
#endif
      FILTER3(candidates.back(), set_domain(scope[n]));	
#ifdef _DEBUG_MIN
      if(IS_OK(wiped)) {
	std::cout << "    => " << scope[candidates.back()] << " in " << scope[candidates.back()].get_domain() << std::endl;
      } else {
	std::cout << "    => fail!" << std::endl;
      }
#endif
    } else {
      if(LB_CHANGED(evt)) {
#ifdef _DEBUG_MIN
	std::cout << "    => update " << candidates << "'s lb" << std::endl;
#endif
	for(i=candidates.size; IS_OK(wiped) && i;) {
	  --i;
	  var = candidates[i];
	  FILTER3(var, set_min(scope[n].get_min()));	
#ifdef _DEBUG_MIN
	  if(IS_OK(wiped)) {
	    std::cout << "    => " << scope[var] << " in " << scope[var].get_domain() << std::endl;
	  } else {
	    std::cout << "    => fail!" << std::endl;
	  }
#endif
	}
      } 
      if(UB_CHANGED(evt)) {
	ub = scope[n].get_max();
#ifdef _DEBUG_MIN
	std::cout << "    => update " << candidates << std::endl;
#endif
	for(i=candidates.size; i;) {
	  --i;
	  var = candidates[i];
	  if(scope[var].get_min() > ub) {
#ifdef _DEBUG_MIN
	    std::cout << "      => remove " << scope[var] << " in " << scope[var].get_domain() 
		      << " (" << scope[var].get_min() << " > " << ub << ")" << std::endl;
#endif
	    candidates.reversible_remove(var);
	  }
	}
	if(candidates.empty()) wiped = FAILURE(n);
#ifdef _DEBUG_MIN
	if(IS_OK(wiped)) {
	  std::cout << "    => " << candidates << std::endl;
	} else {
	  std::cout << "    => fail!" << std::endl;
	}
#endif
      }
    }
  }

  if(cs>1 && candidates.size == 1) {
#ifdef _DEBUG_MIN
    std::cout << "  => only one candidate! (enforce equality)" << std::endl;
#endif
    var = candidates.back();
    if(IS_OK(wiped)) FILTER3(var, set_domain(scope[n]));

#ifdef _DEBUG_MIN
	  if(IS_OK(wiped)) {
	    std::cout << "    => " << scope[var] << " in " << scope[var].get_domain() << std::endl;
	  } else {
	    std::cout << "    => fail!" << std::endl;
	  }
#endif

    if(IS_OK(wiped)) FILTER3(n, set_domain(scope[var]));

#ifdef _DEBUG_MIN
	  if(IS_OK(wiped)) {
	    std::cout << "    => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
	  } else {
	    std::cout << "    => fail!" << std::endl;
	  }
#endif
  }
}

// Mistral::PropagationOutcome Mistral::PredicateMin::propagate(const int changed_idx, const Event evt) {
//   PropagationOutcome wiped = CONSISTENT;
//   react_to(wiped, changed_idx, evt);
//   return wiped;
// }

// 'candidates' stores the indices of the variables that could possibly take the minimum value


Mistral::PropagationOutcome Mistral::PredicateMin::propagate() {

  PropagationOutcome wiped = CONSISTENT;
  int var;
  Event evt;

#ifdef _DEBUG_MIN
  std::cout << "\nbeg propagate " << (*this) << std::endl;
#endif

  while(IS_OK(wiped) && !changes.empty()) {
    var = changes.pop();
    evt = event_type[var];
    event_type[var] = NO_EVENT;
    
    react_to(wiped, var, evt);
  }

#ifdef _DEBUG_MIN
  std::cout << "end propagate " << (*this) << std::endl;
#endif

  return wiped;



//   PropagationOutcome wiped = CONSISTENT;
//   unsigned int n = scope.size-1, evt;
//   int i, val, aux, var; 
//   Event evt_ty;


// #ifdef _DEBUG_MIN
//   if(_DEBUG_MIN) {
//     std::cout << std::endl << active << std::endl << candidates << std::endl << changes << std::endl;
//   }
// #endif

//   if(candidates.size != 1) {
//     while(IS_OK(wiped) && !changes.empty()) {
//       if(changes.contain(n)) { 
// #ifdef _DEBUG_MIN
// 	if(_DEBUG_MIN) {
// 	  std::cout << " " << scope[n] << " has changed "  << event2str(event_type[n]) << std::endl;
// 	}
// #endif
// 	changes.remove(n);
// 	evt_ty = event_type[n];
// 	event_type[n] = NO_EVENT;
// 	// the "self" variable has changed.
// 	// if the lower bound has been changed, we need to update the lb of every active variable
// 	if(LB_CHANGED(evt_ty)) {
// 	  val = scope[n].get_min();
// 	  for(i=candidates.size; IS_OK(wiped) && --i>=0; ) {
// 	    var = candidates[i];
// 	    if(scope[var].get_min() < val) {
// #ifdef _DEBUG_MIN
// 	      if(_DEBUG_MIN) {
// 		std::cout << " => " << scope[var] << " in " << scope[var].get_domain() << " >= " << val << std::endl;
// 	      }
// #endif
// 	      event_type[var] |= scope[var].set_min(val);
// 	      if(FAILED(event_type[var])) wiped = FAILURE(var);
// 	      else if(!changes.contain(var)) {
// #ifdef _DEBUG_MIN
// 		if(_DEBUG_MIN) {
// 		  std::cout << "    " << scope[var] << " has changed: " << scope[var].get_domain() << std::endl;
// 		}
// #endif
// 		changes.add(var);
// 		if((scope[var].get_min() > scope[n].get_max()) && candidates.contain(var)) {
// 		  candidates.reversible_remove(var);
// #ifdef _DEBUG_MIN
// 		  if(_DEBUG_MIN) {
// 		    std::cout << "    " << scope[var] << " can no longer be min" << std::endl;
// 		  }
// #endif
// 		}
// 	      }	
// 	    }
// 	  }
// 	}
// 	// if the upper bound has been changed, some active variables may become unactive
// 	// The min(maximum) might also change because variables became unactive
// 	if(IS_OK(wiped) && UB_CHANGED(evt_ty)) {
// 	  aux = val = scope[n].get_max();
// #ifdef _DEBUG_MIN
// 	  if(_DEBUG_MIN) {
// 	    std::cout << " relax the constraint from vars greater than " << val << std::endl;
// 	  }
// #endif
// 	  for(i=candidates.size; IS_OK(wiped) && i;) {
// 	    --i;
// 	    var = candidates[i];
// 	    if(scope[var].get_min() > val) {
// #ifdef _DEBUG_MIN
// 	      if(_DEBUG_MIN) {
// 		std::cout << "    relax1 " << this << " from " << scope[var] << " in " << scope[var].get_domain() << std::endl;
// 	      }
// #endif 
// 	      if(candidates.contain(var)) candidates.reversible_remove(var);
// 	    } else if(aux > scope[var].get_max()) aux = scope[var].get_max();
// 	  }
// #ifdef _DEBUG_MIN
// 	  if(_DEBUG_MIN) {
// 	    std::cout << " => " << scope[n] << " in " << scope[n].get_domain() << " <= " << aux << std::endl;
// 	  }
// #endif
// 	  if(aux < scope[n].get_max()) {
// 	    event_type[n] |= scope[n].set_max(aux);
// 	    if(FAILED(event_type[n])) wiped = FAILURE(n);
// 	    else if(!changes.contain(n)) changes.add(n); 
// 	  }
// 	}
//       } 
//       if(IS_OK(wiped)) {
// 	// store the min min of the changed vars in aux, and the min max in val
// 	aux = INFTY;
// 	val = INFTY;
// 	while(!changes.empty()) {
// 	  evt = changes.pop();
// 	  evt_ty = event_type[evt];
// 	  event_type[evt] = NO_EVENT;
// #ifdef _DEBUG_MIN
// 	  if(_DEBUG_MIN) {
// 	    std::cout << "-event on " << scope[evt] 
// 		      << (LB_CHANGED(evt_ty) ? " (lb) " : " ")
// 		      << (UB_CHANGED(evt_ty) ? "(ub) " : " ")
// 		      << std::endl;
// 	  }
// #endif

// 	  if(candidates.contain(evt)) {
// 	    if(LB_CHANGED(evt_ty) 
// 	       && scope[evt].get_min()<aux
// 	       ) {
// 	      aux = scope[evt].get_min();
// 	      if(aux > scope[n].get_max()) {
// #ifdef _DEBUG_MIN
// 		if(_DEBUG_MIN) {
// 		  std::cout << "    relax2 " << this << " from " << scope[evt] << " in " << scope[evt].get_domain() << std::endl;
// 		}
// #endif 
// 		candidates.reversible_remove(evt);
// 	      }
// 	    }
// 	    if(UB_CHANGED(evt_ty) && scope[evt].get_max()<val)
// 	      val = scope[evt].get_max();
// 	  }
// 	}
// #ifdef _DEBUG_MIN
// 	if(_DEBUG_MIN) {
// 	  if(val < INFTY) 
// 	    std::cout << " New min maximum: " << val << std::endl;
// 	  if(aux < INFTY) 
// 	    std::cout << " New min minimum: " << aux << std::endl;
// 	}
// #endif
// 	if(val < scope[n].get_max()) {
// #ifdef _DEBUG_MIN
// 	  if(_DEBUG_MIN) {
// 	    std::cout << " => " << scope[n] << " in " << scope[n].get_domain() 
// 		      << " <= " << val << std::endl;
// 	  }
// #endif
// 	  event_type[n] |= scope[n].set_max(val);
// 	  if(FAILED(event_type[n])) wiped = FAILURE(n);
// 	  else if(!changes.contain(n)) changes.add(n);
// 	}
// 	if(aux < INFTY) {
// 	  val = scope[n].get_min();
// #ifdef _DEBUG_MIN
// 	  if(_DEBUG_MIN) {
// 	    std::cout << " previous min minimum witness: " << scope[last_min] 
// 		      << " in " << scope[last_min].get_domain() << std::endl;
// 	  }
// #endif
// 	  if(aux > val && scope[last_min].get_min() >  val) {
// 	    // look for a new witness for the min
// 	    i = candidates.size;
// 	    while( i > 0 ) {
// 	      --i;
// 	      var = scope[candidates[i]].get_min();
// 	      if(aux > var) aux = var;
// 	      if(var <= val) {
// 		last_min = candidates[i];
// 		break;
// 	      } 
// 	    }
// 	    if(aux > val) {
// #ifdef _DEBUG_MIN
// 	      if(_DEBUG_MIN) {
// 		std::cout << " => " << scope[n] << " in " << scope[n].get_domain() 
// 			  << " >= " << aux << std::endl;
// 	      }
// #endif
// 	      event_type[n] |= scope[n].set_min(aux);
// 	      if(FAILED(event_type[n])) wiped = FAILURE(n);
// 	      else if(!changes.contain(n)) changes.add(n);
// 	    }
// 	  } else {
// #ifdef _DEBUG_MIN
// 	    if(_DEBUG_MIN) {
// 	      std::cout << " no need to update " << scope[n] << "'s min" << std::endl;
// 	    }
// #endif
// 	  }
// 	}
//       }
//     }  
//   }

//   if(candidates.size == 1) {

// #ifdef _DEBUG_MIN
//     if(_DEBUG_MIN) {
//       std::cout << scope[candidates[0]] << " is the last min var " << std::endl;
//     }
// #endif

//     if(scope[candidates[0]].set_domain(scope[n]) == FAIL_EVENT) wiped = FAILURE(candidates[0]);
//     if(scope[n].set_domain(scope[candidates[0]]) == FAIL_EVENT) wiped = FAILURE(n);
//   }


//   return wiped;
}

std::ostream& Mistral::PredicateMin::display(std::ostream& os) const {
  os << scope.back() << " == min(" << scope[0]/*.get_var()*/;
  for(unsigned int i=1; i<scope.size-1; ++i) {
    os << ", " << scope[i]/*.get_var()*/ ;
  }
  os << ")";
  return os;
}




Mistral::PredicateMax::PredicateMax(Vector< Variable >& scp) : GlobalConstraint(scp) { priority = 1; }

Mistral::PredicateMax::~PredicateMax() {
#ifdef _DEBUG_MEMORY
  std::cout << "c delete max predicate" << std::endl;
#endif
}

void Mistral::PredicateMax::initialise() {
  ConstraintImplementation::initialise();
  last_max = 0;
  for(unsigned int i=0; i<scope.size; ++i) {
    trigger_on(_RANGE_, scope[i]);
    if(scope[i].get_max() > scope[last_max].get_max())
      last_max = i;
  }

  GlobalConstraint::initialise();


//   //std::cout << id << std::endl;
// #ifdef _DEBUG_MAX
//   for(int i=0; i<scope.size-1; ++i) {
//       std::cout << scope[i] << " in " << scope[i].get_domain() << std::endl;
//   }
// #endif


  candidates.initialise(solver, 0, scope.size-2, scope.size-1, true);
  int n = scope.size-1;
  for(int i=0; i<n; ++i) {
    if(scope[i].get_max() < scope[n].get_min()) {
      candidates.reversible_remove(i);
      //relax_from(i);
      if(!scope[i].is_ground()) un_post_from(i);
    }
  }

  enforce_nfc1 = false;
  //set_idempotent(true);
}

void Mistral::PredicateMax::mark_domain() {
  for(int i=scope.size; i;)
    get_solver()->forbid(scope[--i].id(), LIST_VAR);
}

int Mistral::PredicateMax::check( const int* sol ) const {
  int i=scope.size-1, the_max=sol[0], smax = sol[i];
  while( --i > 0 ) {
    if(sol[i] > the_max) the_max = sol[i];
    if(the_max > smax) break;
  }
  return the_max != smax;
}


void Mistral::PredicateMax::react_to(PropagationOutcome& wiped, const int changed_idx, const Event evt) {
  //PropagationOutcome wiped = CONSISTENT;
  int n = scope.size-1, i, var, ub, lb;
  int cs = candidates.size;

#ifdef _DEBUG_MAX
  if(_DEBUG_MAX) {
    std::cout << "react to " << event2str(evt) << " on " << scope[changed_idx] << " in " 
	      << scope[changed_idx].get_domain() << std::endl;
  }
#endif
  
  if(changed_idx<n) {
    if(candidates.contain(changed_idx)) {
#ifdef _DEBUG_MAX
      if(_DEBUG_MAX) {
	std::cout << "  " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << " is a candidate" << std::endl;
      }
#endif
      if(candidates.size == 1) { // equality
#ifdef _DEBUG_MAX
	if(_DEBUG_MAX) {
	  std::cout << "    => the only one! (enforce equality)" << std::endl;
	}
#endif
	FILTER3(n, set_domain(scope[changed_idx]));
#ifdef _DEBUG_MAX
	if(_DEBUG_MAX) {
	  if(IS_OK(wiped)) {
	    std::cout << "    => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
	  } else {
	    std::cout << "    => fail!" << std::endl;
	  }
	}
#endif
      } else {// scope[i] >= scope[n]
	if(LB_CHANGED(evt)) {
#ifdef _DEBUG_MAX
	  if(_DEBUG_MAX) {
	    std::cout << "    => update " << scope[n] << "'s lb" << std::endl;
	  }
#endif
	  FILTER3(n, set_min(scope[changed_idx].get_min()));	
#ifdef _DEBUG_MAX
	  if(_DEBUG_MAX) {
	    if(IS_OK(wiped)) {
	      std::cout << "    => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
	    } else {
	      std::cout << "    => fail!" << std::endl;
	    }
	  }
#endif
	} 
	if(UB_CHANGED(evt)) {
	  
	  if(scope[n].get_min() > scope[changed_idx].get_max()) { 
#ifdef _DEBUG_MAX
	    if(_DEBUG_MAX) {
	      std::cout << "    => update " << scope[changed_idx] << "'s possibility" << std::endl;
	    }
#endif
	    // scope[i] may not be the minimum variable anymore?
	    candidates.reversible_remove(changed_idx);
#ifdef _DEBUG_MAX
	    if(_DEBUG_MAX) {
	      std::cout << "    => " << candidates << std::endl;
	    }
#endif
	  } 
	  if(scope[n].get_max() > scope[changed_idx].get_max()) {
#ifdef _DEBUG_MAX
	    if(_DEBUG_MAX) {
	      std::cout << "    => " << scope[changed_idx] << " was (maybe) the witness for " 
			<< scope[n] << "'s max" << std::endl;
	    }
#endif
	    ub = -INFTY;
	    for(i=candidates.size; i;) {
	      --i;
	      var = candidates[i];
	      if(scope[var].get_max() > ub) {
		ub = scope[var].get_max();
	      }
	    }
#ifdef _DEBUG_MAX
	    if(_DEBUG_MAX) {
	      std::cout << "      => new ub = " << ub << std::endl;
	    }
#endif
	    FILTER3(n, set_max(ub));
	  }
	}
      }
    }
#ifdef _DEBUG_MAX
    else {
      if(_DEBUG_MAX) {
	std::cout << "  " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << " is not a candidate" << std::endl;
      }
    }
#endif
  } else {
#ifdef _DEBUG_MAX
    if(_DEBUG_MAX) {
      std::cout << "  " << scope[changed_idx] << " in " << scope[changed_idx].get_domain() << " is the MAX" << std::endl;
    }
#endif
    if(candidates.size == 1) { // equality
#ifdef _DEBUG_MAX
      if(_DEBUG_MAX) {
	std::cout << "    => only one candidate (enforce equality)" << std::endl;
      }
#endif

      FILTER3(candidates.back(), set_domain(scope[n]));

#ifdef _DEBUG_MAX
      if(_DEBUG_MAX) {
	if(IS_OK(wiped)) {
	  std::cout << "    => " << scope[candidates.back()] << " in " << scope[candidates.back()].get_domain() << std::endl;
	} else {
	  std::cout << "    => fail!" << std::endl;
	}
      }
#endif
    } else {
      if(UB_CHANGED(evt)) {
#ifdef _DEBUG_MAX
	if(_DEBUG_MAX) {
	  std::cout << "    => update " << candidates << "'s ub" << std::endl;
	}
#endif
	for(i=candidates.size; IS_OK(wiped) && i;) {
	  --i;
	  var = candidates[i];
	  FILTER3(var, set_max(scope[n].get_max()));	
#ifdef _DEBUG_MAX
	  if(_DEBUG_MAX) {
	    if(IS_OK(wiped)) {
	      std::cout << "    => " << scope[var] << " in " << scope[var].get_domain() << std::endl;
	    } else {
	      std::cout << "    => fail!" << std::endl;
	    }
	  }
#endif
	}
      } 
      if(LB_CHANGED(evt)) {
	lb = scope[n].get_min();
#ifdef _DEBUG_MAX
	if(_DEBUG_MAX) {
	  std::cout << "    => update " << candidates << std::endl;
	}
#endif
	for(i=candidates.size; i;) {
	  --i;
	  var = candidates[i];
	  if(scope[var].get_max() < lb) {
#ifdef _DEBUG_MAX
	    if(_DEBUG_MAX) {
	      std::cout << "      => remove " << scope[var] << " in " << scope[var].get_domain() 
			<< " (" << scope[var].get_max() << " < " << lb << ")" << std::endl;
	    }
#endif
	    candidates.reversible_remove(var);
	  }
	}
	if(candidates.empty()) wiped = FAILURE(n);
#ifdef _DEBUG_MAX
	if(_DEBUG_MAX) {
	  if(IS_OK(wiped)) {
	    std::cout << "    => " << candidates << std::endl;
	  } else {
	    std::cout << "    => fail!" << std::endl;
	  }
	}
#endif
      }
    }
  }

  if(cs>1 && candidates.size == 1) {
#ifdef _DEBUG_MAX
    if(_DEBUG_MAX) {
      std::cout << "  => only one candidate! (enforce equality)" << std::endl;
    }
#endif
    var = candidates.back();
    if(IS_OK(wiped)) FILTER3(var, set_domain(scope[n]));

#ifdef _DEBUG_MAX
    if(_DEBUG_MAX) {
      if(IS_OK(wiped)) {
	std::cout << "    => " << scope[var] << " in " << scope[var].get_domain() << std::endl;
      } else {
	std::cout << "    => fail!" << std::endl;
      }
    }
#endif

    if(IS_OK(wiped)) FILTER3(n, set_domain(scope[var]));

#ifdef _DEBUG_MAX
    if(_DEBUG_MAX) { 
      if(IS_OK(wiped)) {
	std::cout << "    => " << scope[n] << " in " << scope[n].get_domain() << std::endl;
      } else {
	std::cout << "    => fail!" << std::endl;
      }
    }
#endif
  }
}


Mistral::PropagationOutcome Mistral::PredicateMax::propagate() {


  PropagationOutcome wiped = CONSISTENT;
  int var;
  Event evt;

#ifdef _DEBUG_MAX
 if(_DEBUG_MAX) { 
  std::cout << "\nbeg propagate " << (*this) << std::endl;
 }
#endif

  while(IS_OK(wiped) && !changes.empty()) {
    var = changes.pop();
    evt = event_type[var];
    event_type[var] = NO_EVENT;
    
    react_to(wiped, var, evt);
  }

#ifdef _DEBUG_MAX
 if(_DEBUG_MAX) { 
  std::cout << "end propagate " << (*this) << std::endl;
 }
#endif

  return wiped;


//   PropagationOutcome wiped = CONSISTENT;
//   unsigned int n = scope.size-1, evt;
//   int i, val, aux, var; 

// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
//   for(i=0; i<changes.size; ++i) {
//     std::cout << event2str(event_type[changes[i]]) << " " 
// 	      << LB_CHANGED(event_type[changes[i]]) << " " 
// 	      << UB_CHANGED(event_type[changes[i]]) << std::endl;
//   }
//   }
// #endif

// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
//   std::cout << "propagate max( " ;
//   for(i=0; i<n; ++i) 
//     std::cout << "[" << scope[i].get_min() << "," << scope[i].get_max() << "] ";
//   std::cout << ") = [" << scope[n].get_min() << "," << scope[n].get_max() << "]" ;

//   std::cout << std::endl << active << std::endl << candidates << " " << changes << std::endl;
//   }
// #endif

//   if(candidates.size != 1) {
//     while(IS_OK(wiped) && !changes.empty()) {
//       if(changes.contain(n)) { 
	
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	std::cout << " " << scope[n] << " has changed " << std::endl;
//   }
// #endif
	
// 	changes.remove(n);
// 	// the "self" variable has changed.
	
// 	// if the upper bound has been changed, we need to update the ub of every active variable
// 	if(UB_CHANGED(event_type[n])) {
// 	  val = scope[n].get_max();
// 	  for(i=candidates.size; IS_OK(wiped) && --i>=0; ) {

// 	    var = candidates[i];

// 	    if(scope[var].get_max() > val) {
// #ifdef _DEBUG_MAX
// 	      if(_DEBUG_MAX)
// 		{
// 		  std::cout << " => " << scope[var] << " in " << scope[var].get_domain() << " <= " << val << std::endl;
// 		}
// #endif
// 	      event_type[var] |= scope[var].set_max(val);
// 	      //if(event_type[var] == FAIL_EVENT) wiped = FAILURE(var);
// 	      if(FAILED(event_type[var])) wiped = FAILURE(var);
// 	      else if(!changes.contain(var)) {
// #ifdef _DEBUG_MAX
// 		if(_DEBUG_MAX)
// 		  {
// 		    std::cout << "    " << scope[var] << " has changed: " << scope[var].get_domain() << std::endl;
// 		  }
// #endif
// 		changes.add(var);
// 		if((scope[var].get_max() < scope[n].get_min()) && candidates.contain(var)) {
// 		  candidates.reversible_remove(var);
// #ifdef _DEBUG_MAX
// 		  if(_DEBUG_MAX)
// 		    {
// 		      std::cout << "    " << scope[var] << " can no longer be max" << std::endl;
// 		    }
// #endif
// 		}
// 	      }
// 	    }
// 	  }
// 	}
	
// 	// if the lower bound has been changed, some active variables may become unactive
// 	// The max(maximum) might also change because variables became unactive
// 	if(IS_OK(wiped) && LB_CHANGED(event_type[n])) {
// 	  aux = val = scope[n].get_min();
	  
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	  std::cout << " relax the constraint from vars lower than " << val << std::endl;
//   }
// #endif
	  
// 	  for(i=candidates.size; IS_OK(wiped) && i;) {
// 	    --i;
// 	    var = candidates[i];
	    
// 	    //std::cout << i << ": " << 
	    
// 	    if(scope[var].get_max() < val) {
	      
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	      std::cout << "    relax1 " << this << " from " << scope[var] << std::endl;
//   }
// #endif 
	      
// 	      //relax_from(var);
// 	      if(candidates.contain(var)) candidates.reversible_remove(var);
// 	      //std::cout << candidates << std::endl;
	      
// 	    } else if(aux < scope[var].get_min()) aux = scope[var].get_min();
// 	  }
	  
// #ifdef _DEBUG_MAX
// 	  if(_DEBUG_MAX)
// 	    {
// 	      std::cout << " => " << scope[n] << " in " << scope[n].get_domain() << " <= " << aux << std::endl;
// 	    }
// #endif
	  
// 	  if(scope[n].set_min(aux) == FAIL_EVENT) wiped = FAILURE(n);	
// 	}
//       } 
      
//       if(IS_OK(wiped)) {
	
// 	// store the max max of the changed vars in aux, and the max max in val
// 	aux = -INFTY;
// 	val = -INFTY;
// 	while(!changes.empty()) {
// 	  evt = changes.pop();
	  
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	  std::cout << "-event on " << scope[evt] 
// 		    << (LB_CHANGED(event_type[evt]) ? " (lb) " : " ")
// 		    << (UB_CHANGED(event_type[evt]) ? "(ub) " : " ")
// 		    << std::endl;
//   }
// #endif
	  
// 	  if(UB_CHANGED(event_type[evt]) 
// 	     && scope[evt].get_max()>aux
// 	     ) {
// 	    aux = scope[evt].get_max();
// 	    if(aux < scope[n].get_min()) {
	      
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	      std::cout << "    relax2 " << this << " from " << scope[evt] << std::endl;
//   }
// #endif 

// 	      //relax_from(evt);
// 	      if(candidates.contain(evt)) candidates.reversible_remove(evt);
// 	      //std::cout << candidates << std::endl;
// 	    }
// 	  }
// 	  if(LB_CHANGED(event_type[evt]) && scope[evt].get_min()>val)
// 	    val = scope[evt].get_min();
// 	  //if(ASSIGNED(event_type[evt]))
// 	  //candidates.reversible_remove(evt);
// 	}
	
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	if(val > -INFTY) 
// 	  std::cout << " New min maximum: " << val << std::endl;
// 	if(aux > -INFTY) 
// 	  std::cout << " New max maximum: " << aux << std::endl;
//   }
// #endif
      
      
// 	if(val > -INFTY) {
	
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	  std::cout << " => " << scope[n] << " in " << scope[n].get_domain() 
// 		    << " >= " << val << std::endl;
//   }
// #endif
	
// 	  event_type[n] = scope[n].set_min(val);
// 	  if(event_type[n] == FAIL_EVENT) wiped = FAILURE(n);
// 	  else {
// 	    changes.add(n);
// 	  }
// 	}
	
// 	if(aux > -INFTY) {
// 	  val = scope[n].get_max();
	  
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	  std::cout << " previous max minimum witness: " << scope[last_max] 
// 		    << " in " << scope[last_max].get_domain() << std::endl;
//   }
// #endif
      
// 	  if(aux < val && scope[last_max].get_max() < val) {
	  
// 	    // std::cout << candidates << std::endl;
// 	    // for(i=0; i<scope.size; ++i) {
// 	    //   std::cout << " " << scope[i].get_domain();
// 	    // }
// 	    // std::cout << std::endl;
	    
// 	    // look for a new witness for the max
// 	    i = candidates.size;
// 	    while( i > 0 ) {
// 	      --i;
	      
// 	      // std::cout << i << std::endl;
	      
// 	      // std::cout << candidates[i] << std::endl;
	      
// 	      var = scope[candidates[i]].get_max();
// 	      if(aux < var) aux = var;
// 	      if(var >= val) {
// 		last_max = candidates[i];
// 		break;
// 	      } 
// 	    }
	  
// 	    if(aux < val) {
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	      std::cout << " => " << scope[n] << " in " << scope[n].get_domain() 
// 			<< " <= " << aux << std::endl;
//   }
// #endif
// 	      event_type[n] = scope[n].set_max(aux);
// 	      if(event_type[n] == FAIL_EVENT) wiped = FAILURE(n);
// 	      else {
// 		if(!changes.contain(n)) changes.add(n);
// 	      }
// 	    }
// 	  } else {
	    
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
// 	    std::cout << " no need to update " << scope[n] << "'s max" << std::endl;
//   }
// #endif
	    
// 	  }
// 	}
//       }
//     }  
//   }

// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
//   std::cout << candidates << std::endl;
//   }
// #endif


//   if(candidates.size == 1) {
    
// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//   {
//     std::cout << scope[candidates[0]] << " is the last max var " << std::endl;
//   }
// #endif
    
//     if(scope[candidates[0]].set_domain(scope[n]) == FAIL_EVENT) wiped = FAILURE(candidates[0]);
//     if(scope[n].set_domain(scope[candidates[0]]) == FAIL_EVENT) wiped = FAILURE(n);
//   } else if(candidates.empty()) {
//     wiped = n;
//   }

// #ifdef _DEBUG_MAX
//   if(_DEBUG_MAX)
//     {
//       std::cout << wiped << " max( ";
//       for(int k=0; k<scope.size-1; ++k)
// 	std::cout // << scope[k] << " in " 
// 	  << scope[k].get_domain() << " ";
//       std::cout << ") = " // << scope[scope.size-1] << " in "
// 		<< scope[scope.size-1].get_domain() << std::endl << std::endl;
//     }
// #endif
  
//   return wiped;
}

std::ostream& Mistral::PredicateMax::display(std::ostream& os) const {
  os << scope.back()/*.get_var()*/ << " == max(" << scope[0]/*.get_var()*/;
  for(unsigned int i=1; i<scope.size-1; ++i) {
    os << ", " << scope[i]/*.get_var()*/ ;
  }
  os << ")";
  return os;
}


// // Mistral::ConstraintNogoodBase::ConstraintNogoodBase(Vector< Variable >& scp) 
// //   : Constraint(scp) { }

// // void Mistral::ConstraintNogoodBase::mark_domain() {
// //   for(unsigned int i=0; i<scope.size; ++i) {
// //     solver->mark_non_convex(scope[i].id());
// //   }
// // }

// // void Mistral::ConstraintNogoodBase::initialise() {
// //   Constraint::initialise();
// //   //int min_idx = INFTY;
// //   //int max_idx = -INFTY;
// //   for(unsigned int i=0; i<scope.size; ++i) {
// //     //if(scope[i].get_id() > max_idx) max_idx = scope[i].get_id();
// //     //if(scope[i].get_id() < min_idx) min_idx = scope[i].get_id();
// //     trigger_on(_VALUE_, i);
// //   }
// //   set_idempotent(true);

// //   //if(// min_idx < 
// //   //max_idx) {
// //   watch_structure.initialise(0,scope.size);
// //     //}

// //   for(unsigned int i=0; i<scope.size; ++i) {
// //     //add(scope[i]);
// //     watch_structure[i] = new Vector< Array< Literal >* > [scope[i].get_size()];
// //     watch_structure[i]-=scope[i].get_min();
// //   }
// // }

// // Mistral::ConstraintNogoodBase::~ConstraintNogoodBase() {
// // }

// // void Mistral::ConstraintNogoodBase::add(Variable x) {
// //   unsigned int idx = x.id();
// //   if(idx >= scope.size) {
// //     while(scope.capacity <= idx)
// //       scope.extendStack();
// //     scope[idx] = x;
// //     while(watch_structure.capacity <= idx)
// //       watch_structure.extendStack();
// //   }
// // }

// // void Mistral::ConstraintNogoodBase::add( Vector < Literal >& clause ) {
// //  if(clause.size > 1) {
// //    Array<Literal> *cl = Array<Literal>::Array_new(clause);
// //    nogood.add( cl );
// //    // std::cout << clause[0].get_atom() << " " << (1-clause[0].get_value()) << std::endl;
// //    // std::cout << watch_structure[clause[0].get_atom()][1-clause[0].get_value()] << std::endl;

// //    watch_structure[clause[0].get_atom()][1-clause[0].get_value()].add(cl);
// //    watch_structure[clause[1].get_atom()][1-clause[1].get_value()].add(cl);
// //  } else {
// //    clause[0].apply(scope[clause[0].get_atom()]);
// //  }
// // }

// // int Mistral::ConstraintNogoodBase::check( const int* sol ) const {
// //   unsigned int i, j;
// //   bool satisfied=true;

// //   for(i=0; i<nogood.size; ++i) {
// //     satisfied = true;
// //     Array < Literal >& clause = *(nogood[i]);
// //     for(j=0; satisfied && j<clause.size; ++j) {
// //       satisfied = clause[j].check(sol[j]);
// //     }
// //   }
  
// //   return !satisfied;
// // }

// // Mistral::PropagationOutcome Mistral::ConstraintNogoodBase::propagate() {
// //   Array< Literal > *conflict=NULL;

// //   int x, v, cw;
// //   Literal p;
// //   //unsigned int i;
// //   while( !changes.empty() ) {
// //     std::cout << changes << std::endl;
// //     //std::cout << scope << std::endl;
// //     x = changes.pop();
// //     //std::cout << scope[x] << " in " << scope[x].get_domain() << std::endl;
// //     v = scope[x].get_min();
// //     std::cout << x << "=" << v << ": " << watch_structure[x][v] << std::endl;

// //     p = Literal(x,EQ,v);


// //     cw = watch_structure[x][v].size;
// //     while(cw-- && !conflict) {
// //       conflict = update_watcher(cw, x, v);
// //     }
// //   }

// //   return CONSISTENT;
// // }


// // inline Array< Literal >* SatSolver::update_watcher(const int cw, const int x, const int v)
// // {
// //   Array< Literal > *cl = watch_structure[x][v][cw];
// //   Array< Literal >& clause = *cl;


// //   unsigned int j;
// //   Lit q, r;
// //   Atom v, w;

// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// //   std::cout << " ";
// //   print_clause( std::cout, watch_structure[p][cw] );
// //   std::cout << std::endl;
// // #endif

// //   //ensure that p is the second watched lit
// //   if( clause[1] != p ) {
// //     q = clause[1];
// //     clause[0] = q;
// //     clause[1] = p;
// //   } else q = clause[0];
// //   v=state[UNSIGNED(q)];

// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// //    std::cout << " second watched: ";
// //    printLiteral(std::cout, q);
// //    std::cout << std::endl;

// //    if( LEVEL(v) < assumptions.size && SIGN(v) == SIGN(q) ) {
// //      for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// //      std::cout << " satisfied by second watched" << std::endl;
// //    }
// // #endif

// //   //check if the other watched lit is assigned
// //   if( LEVEL(v) >= assumptions.size || SIGN(v) != SIGN(q) ) {
// //     for(j=2; j<clause.size; ++j) {
// //       // for each literal q of the clause,
// //       r = clause[j];
// //       w = state[UNSIGNED(r)];

// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// //        std::cout << "\tcheck another lit: ";
// //        printLiteral(std::cout, r);
// //        std::cout << std::endl;
// // #endif

// //       if( LEVEL(w) >= assumptions.size ) { // this literal is not set
// // 	// then it is a good candidate to replace p
// // 	clause[1] = r;
// // 	clause[j] = p;
// // 	watch_structure[p].remove(cw);
// // 	watch_structure[r].add(cl);

// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// // 	std::cout << " new watched" << std::endl;
// // #endif

// // 	break;	
// //       }
// //       // if it is set true, then the clause is satisfied
// //       else if( SIGN(w) == SIGN(r) ) {

// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// // 	std::cout << " satisfied" << std::endl;
// // #endif

// // 	break;
// //       }
// //     }
      
// //     if( j == clause.size ) // no replacement could be found
// //       { 

// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// // 	std::cout << " no more watched" << std::endl;
// // #endif

// // 	if( LEVEL(v) >= assumptions.size ) {
// // 	  // the last literal (other watched lit) is not set yet, we set it
// // 	  add_lit(q);
// // 	  reason[UNSIGNED(q)] = cl;

// // 	  //#ifdef _DEBUGNOGOOD
// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// // 	  std::cout //<< "unit prune disjunct b" << y 
// // 	    << " because ";
// // 	  print_clause( std::cout, cl );
// // 	  std::cout << std::endl;
// // #endif

// // 	  //	  std::cout << " prop " << q << std::endl;
	  
// // 	} else 
// // 	  // it is set to false already, we fail
// // 	  if( // polarity[y] == -q
// // 	     SIGN(v) != SIGN(q)
// // 	      ) {

// // #ifdef _DEBUG_WATCH
// //   for(unsigned int i=0; i<decisions.size; ++i) std::cout << " " ;
// // 	    std::cout << " fail" << std::endl;
// // #endif

// // 	    return cl;
// // 	  }
// //       }
// //   }

// //   return NULL;
// // }




// // std::ostream& Mistral::ConstraintNogoodBase::display(std::ostream& os) const {
// //   os << " (";
// //   if(nogood.size>0) {
// //     os << nogood[0];
// //     for(unsigned int i=1; i<nogood.size; ++i)
// //       os << " " << nogood[i]  ;
// //   }
// //   os << ")";
// //   return os;
// // }



// // Mistral::ConstraintClauseBase::ConstraintClauseBase(Vector< Variable >& scp) 
// //   : Constraint(scp) { }

// // void Mistral::ConstraintClauseBase::mark_domain() {
// //   for(unsigned int i=0; i<scope.size; ++i) {
// //     solver->mark_non_convex(scope[i].id());
// //   }
// // }

// // void Mistral::ConstraintClauseBase::initialise() {
// //   Constraint::initialise();
// //   for(unsigned int i=0; i<scope.size; ++i) {
// //     trigger_on(_VALUE_, i);
// //   }
// //   set_idempotent(true);

// //   is_watched_by.initialise(0,2*scope.size);


// //   // for(unsigned int i=0; i<scope.size; ++i) {
// //   //   is_watched_by[i] = new Vector< Clause* >;
// //   //   is_watched_by[i]-=scope[i].get_min();
// //   // }
// // }

// // Mistral::ConstraintClauseBase::~ConstraintClauseBase() {
// // }

// // void Mistral::ConstraintClauseBase::add(Variable x) {
// //   unsigned int idx = x.id();
// //   if(idx == scope.size) {
// //     scope.add(x);
// //     while(is_watched_by.capacity <= 2*idx)
// //       is_watched_by.extendStack();
// //   } else if(idx > scope.size) {
// //     while(scope.capacity <= idx)
// //       scope.extendStack();
// //     scope[idx] = x;
// //     while(is_watched_by.capacity <= idx)
// //       is_watched_by.extendStack();
// //   }
// // }

// // void Mistral::ConstraintClauseBase::add( Vector < Literal >& clause ) {
// //  if(clause.size > 1) {
// //    Clause *cl = Clause::Array_new(clause);
// //    clause.add( cl );
// //    is_watched_by[clause[0]].add(cl);
// //    is_watched_by[clause[1]].add(cl);
// //  } else {
// //    scope[UNSIGNED(clause[0])].set_domain(SIGN(clause[0]));
// //  }
// // }

// // int Mistral::ConstraintClauseBase::check( const int* sol ) const {
// //   unsigned int i, j;
// //   bool satisfied=true;

// //   for(i=0; i<clause.size; ++i) {
// //     satisfied = true;
// //     Clause& clause = *(clause[i]);
// //     for(j=0; satisfied && j<clause.size; ++j) {
// //       satisfied = //clause[j].check(sol[j]);
// // 	(sol[j] == SIGN(clause[j]));
// //     }
// //   }
  
// //   return !satisfied;
// // }

// // Mistral::PropagationOutcome Mistral::ConstraintClauseBase::propagate() {
// //   Clause *conflict=NULL;


// //   int x, v, cw;
// //   Lit p;
// //   //unsigned int i;
// //   while( !changes.empty() ) {
// //     std::cout << changes << std::endl;
// //     //std::cout << scope << std::endl;
// //     x = changes.pop();
// //     //std::cout << scope[x] << " in " << scope[x].get_domain() << std::endl;
// //     v = scope[x].get_min();
// //     std::cout << x << "=" << v << ": " << is_watched_by[x][v] << std::endl;

// //     p = 2*x+v;

// //     cw = is_watched_by[p].size;
// //     while(cw-- && !conflict) {
// //       conflict = update_watcher(cw, p);
// //     }
// //   }

// //   return CONSISTENT;
// // }


// // inline Clause* SatSolver::update_watcher(const int cw, const Lit p)
// // {
// //   Clause *cl = is_watched_by[p][cw];
// //   Clause& clause = *cl;

// //   unsigned int j;
// //   Lit q, r;
// //   //Atom v, w;
// //   Variable v, w;

// //   //ensure that p is the second watched lit
// //   if( clause[1] != p ) {
// //     q = clause[1];
// //     clause[0] = q;
// //     clause[1] = p;
// //   } else q = clause[0];
// //   v=scope[UNSIGNED(q)].;


// //   //check if the other watched lit is assigned
// //   //if( LEVEL(v) >= assumptions.size || SIGN(v) != SIGN(q) ) {
// //   if( !v.is_ground() || v.get_min() != SIGN(q) ) {
// //     for(j=2; j<clause.size; ++j) {
// //       // for each literal q of the clause,
// //       r = clause[j];
// //       w = scope[UNSIGNED(r)];

// //       if( !w.is_ground() ) { // this literal is not set
// // 	// then it is a good candidate to replace p
// // 	clause[1] = r;
// // 	clause[j] = p;
// // 	is_watched_by[p].remove(cw);
// // 	is_watched_by[r].add(cl);

// // 	break;	
// //       }
// //       // if it is set true, then the clause is satisfied
// //       else if( w.get_min() == SIGN(r) ) {
// // 	break;
// //       }
// //     }
      
// //     if( j == clause.size ) // no replacement could be found
// //       { 
// // 	if( !v,is_ground() ) {
// // 	  // the last literal (other watched lit) is not set yet, we set it
// // 	  add_lit(q);
// // 	  //reason[UNSIGNED(q)] = cl;
// // 	} else 
// // 	  // it is set to false already, we fail
// // 	  if( v.get_min() != SIGN(q) ) {
// // 	    return cl;
// // 	  }
// //       }
// //   }

// //   return NULL;
// // }




// // std::ostream& Mistral::ConstraintClauseBase::display(std::ostream& os) const {
// //   os << " (";
// //   if(clauses.size>0) {
// //     os << clauses[0];
// //     for(unsigned int i=1; i<clauses.size; ++i)
// //       os << " " << clauses[i]  ;
// //   }
// //   os << ")";
// //   return os;
// // }




// std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::Literal& x) {
//   return x.display(os);
// }

// std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::Literal* x) {
//   return x->display(os);
// }


// std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::ConstraintTriggerArray& x) {
//   return x.display(os);
// }

// std::ostream& Mistral::operator<< (std::ostream& os, const Mistral::ConstraintTriggerArray* x) {
//   return x->display(os);
// }
