

namespace k2 {

enum {
  kFinalSymbol = -1,   /* final-costs are represented as arcs with kFinalSymbol as their
                          label, to the final state (see Fst::final_state). */
  kEpsilon = 0         /* Epsilon, which means "no symbol", is numbered zero,
                        * like in OpenFst. */
};

enum {
  kTopSorted,
  kDeterministic,
  kConnected,
  kArcLabelSorted,  /* true if the arcs leaving each state are sorted on label. */
  kEpsilonFree,  /* Note: epsilons to final-state do not count. */
  kNonempty  // ...
} Properties;


/* Range is what we use when we want (begin,end) indexes into some array.
   `end` is the last element plus one.
   TODO: may store just begin and have the next element be the end.
*/
struct Range {
  int32 begin;
  int32 end;
};

struct Arc {
  int32 src_state;
  int32 dest_state;
  int32 label;  /* 'label' as in a finite state acceptor.  For FSTs, the other
                   label will be present in the aux_label array.  Which of the
                   two represents the input vs. the output can be decided
                   by the user; in general, the one that appears on the arc
                   will be the one that participates in whatever operation
                   you are doing */

  /* Note: the costs are not stored here but outside the Fst object, in some kind
     of array indexed by arc-index.  */

};


/*
  struct Fsa is an unweighted finite-state acceptor (FSA) and is at the core of operations
  on weighted FSA's and finite state transducers (FSTs).  Note: being a final-state is
  represented by an arc with label == kEpsilon to final_state.

  The start-state is always numbered zero and the final-state is always the
  last-numbered state.
 */
struct Fsa {
  // Note: an index into the `arcs` array is called an arc-index.
  std::vector<Arc> arcs;

  // `leaving_arcs` is indexed by state-index, is of length num-states,
  // contains the first arc-index leaving this state (index into `arcs`).
  // The next element of this array gives the end of that range.
  // Note: the final-state is numbered last, and implicitly has no
  // arcs leaving it.
  std::vector<Range> leaving_arcs;


  inline size_t NumStates() { return static_cast<int32>(leaving_arcs.size()); }
};

/*
  Returns true if `fsa` is valid AND satisfies the list of properties
  provided in `properties`.

  TODO: create exhaustive list of what `valid` means, but it basically
  means that the data structures make sense, e.g. `leaving_arcs` and
  `arcs` are consistent, and only epsilon arcs enter the final state
  (note: the final state is special, and such arcs represent


 */
bool CheckProperties(const Fsa &fsa,
                     const Properties &properties,
                     bool die_on_error = false);


/*
  this general-purpose structure conceptually the same as std::vector<std::vector>;
  elements of `ranges` are (begin, end) indexes into `values`.
 */
struct VecOfVec {
  std::vector<Range> ranges;
  std::vector<int32> values;
};

/*
  Computes lists of arcs entering each state (needed for algorithms that traverse
  the Fsa in reverse order).

  Requires that `fsa` be valid and top-sorted, i.e.
  CheckProperties(fsa, KTopSorted) == true.
*/
void GetEnteringArcs(const Fsa &fsa,
                     VecOfVec *entering_arcs);


struct Wfst {
  Fsa core;
  std::vector<int32> aux_label;
};




struct Wfsa {
  Fsa arcs;

  float *weights;  // Weights, one per arc, of the same dim as fsa.arcs.  Not
                   // owned here..  would normally be owned in a torch.Tensor at
                   // the python level.
};
