/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * PLEASE NOTE: This file has been copied and pasted from GEANT4, rel. 4.9.6.cand00
 *
 * original source file:
 * /afs/cern.ch/sw/geant4/releases/geant4.9.6.cand00/share/source/graphics_reps/src/BooleanProcessor.src
 *
 *
 * Update: Riccardo-Maria BIANCHI <rbianchi@cern.ch> - 2012-11-16
 *
 *
 * NOTES:
 *
 * ===================
 * VP1 Customization:
 *
 *  - look into the code for comments "// VP1 change"
 *
 *
*/


/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor                            Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Internal class for executing boolean operations           *
 *           on Polyhedra                                              *
 *                                                                     *
 ***********************************************************************/




/* // VP1 change
//G.Barrand : begin
#define BP_GEANT4
*/

#ifdef BP_GEANT4 //G.Barrand
#include "G4Plane3D.hh"
#include "G4Point3D.hh"
#include "G4Normal3D.hh"
typedef G4Plane3D HVPlane3D;
typedef G4Point3D HVPoint3D;
typedef G4Normal3D HVNormal3D;

#else //BP_HEPVIS

#define ExtNode          HEPVis_ExtNode
#define ExtEdge          HEPVis_ExtEdge
#define ExtFace          HEPVis_ExtFace
#define FaceList         HEPVis_FaceList
#define ExtPolyhedron    HEPVis_ExtPolyhedron
#define BooleanProcessor HEPVis_BooleanProcessor

#define HepPolyhedron SbPolyhedron
#define G4Facet SbFacet

// VP1 change
//#include <HEPVis/SbPlane.h>
#include <VP1HEPVis/SbPlane.h>
typedef HEPVis::SbPlane HVPlane3D;
//---


#endif




//using namespace HepGeom;

//#define BP_DEBUG

//G.Barrand : end

#define INITIAL_SIZE 200
#define CRAZY_POINT  HVPoint3D(-10.e+6, -10.e+6, -10.e+6)
//#define GRANULARITY  10.e+5;
#define GRANULARITY  10.e+5  //G.Barrand : rm the trailing ;

#define SWAP(A,B) w = A; A = B; B = w

#define OP_UNION         0    // Operations
#define OP_INTERSECTION  1
#define OP_SUBTRACTION   2

#define OUT_OF_PLANE     0    // Face vs face statuses
#define ON_PLANE         1
#define INTERSECTION     2
#define EDGE             3
#define NON_PLANAR_FACE  4

#define UNKNOWN_FACE     0    // Face statuses
#define ORIGINAL_FACE   -1
#define NEW_FACE        -2
#define UNSUITABLE_FACE -3
#define DEFECTIVE_FACE  -4



// -------------------------------------------- Simplified STL vector ---
//G.Barrand : begin
#include <vector>
using namespace std;
/*
template<class T>
class vector {
 private:
  int cur_size, max_size;
  T * v;

 public:
  vector(): cur_size(0), max_size(INITIAL_SIZE), v(new T[INITIAL_SIZE]) {}
  ~vector() { delete [] v; }

  void      clear()                 { cur_size = 0; }
  int       size()            const { return cur_size; }
  T &       operator[](int i)       { return v[i]; }
  const T & operator[](int i) const { return v[i]; }
  T &       front()                 { return v[0]; }
  const T & front()           const { return v[0]; }
  T &       back()                  { return v[cur_size-1]; }
  const T & back()            const { return v[cur_size-1]; }
  void      pop_back()              { cur_size--; }
  void      push_back(const T & a) {
    if (cur_size == max_size) {
      T * w     = v;
      max_size *= 2;
      v         = new T[max_size];
      for (int i=0; i<cur_size; i++) v[i] = w[i];
      v[cur_size++] = a;
      delete [] w;
    }else{
      v[cur_size++] = a;
    }
  }
};
*/
//G.Barrand : end

// ---------------------------------------------------- Extended node ---
class ExtNode {
 public:
  HVPoint3D v;
  int        s;

 public:
  ExtNode(HVPoint3D vertex=HVPoint3D(), int status=0)
    : v(vertex), s(status) {}
  ~ExtNode() {}

  ExtNode(const ExtNode & node) : v(node.v), s(node.s) {}

  ExtNode & operator=(const ExtNode & node) {
    if (&node == this) return *this;
    v = node.v;
    s = node.s;
    return *this;
  }
};

// ---------------------------------------------------- Extended edge ---
class ExtEdge {
 public:
  int       i1, i2;           // end points
  int       iface1;           // native face
  int       iface2;           // neighbouring face
  int       ivis;             // visibility: +1 (visible), -1 (invisible)
  int       inext;            // index of next edge

 public:
  ExtEdge(int k1=0, int k2=0, int kface1=0, int kface2=0, int kvis=0) :
    i1(k1), i2(k2), iface1(kface1), iface2(kface2), ivis(kvis), inext(0) {}

  ~ExtEdge() {};

  ExtEdge(const ExtEdge & edge) :
    i1(edge.i1), i2(edge.i2), iface1(edge.iface1), iface2(edge.iface2),
    ivis(edge.ivis), inext(edge.inext) {}

  ExtEdge & operator=(const ExtEdge & edge) {
    if (&edge == this) return *this;
    i1     = edge.i1;
    i2     = edge.i2;
    iface1 = edge.iface1;
    iface2 = edge.iface2;
    ivis   = edge.ivis;
    inext  = edge.inext;
    return *this;
  }

  void invert() {
    int w;
    SWAP(i1, i2);
  }
};

// ---------------------------------------------------- Extended face ---
class ExtFace {
 private:
  std::vector<ExtEdge>& edges; //G.Barrand
 public:
  int        iedges[4];        // indices of original edges
  HVPlane3D plane;            // face plane
  double     rmin[3], rmax[3]; // bounding box
  int        iold;             // head of the list of the original edges
  int        inew;             // head of the list of the new edges
  int        iprev;            // index of previous face
  int        inext;            // index of next face

 public:
  //G.Barrand : ExtFace(int iedge=0) : iold(iedge), inew(0), iprev(iprev), inext(0) {}
  ExtFace(std::vector<ExtEdge>& a_edges,int iedge)
  : edges(a_edges), iold(iedge), inew(0), iprev(0), inext(0) {
    //G.Barrand : initialize arrays to quiet valgrind.
   {for (int i=0; i<4; i++) { iedges[i] = 0; }}
   {for (int i=0; i<3; i++) { rmin[i] = 0; rmax[i] = 0; }}
  }
  ~ExtFace() {}

  ExtFace(const ExtFace & face) :
    edges(face.edges), //G.Barrand
    plane(face.plane), iold(face.iold), inew(face.inew),
    iprev(face.iprev), inext(face.inext)
  {
    int i;
    for (i=0; i<4; i++) { iedges[i] = face.iedges[i]; }
    for (i=0; i<3; i++) { rmin[i] = face.rmin[i]; rmax[i] = face.rmax[i]; }
  }

  ExtFace & operator=(const ExtFace & face) {
    if (&face == this) return *this;
    //FIXME : edges(face.edges) ???? //G.Barrand
    int i;
    for (i=0; i<4; i++) { iedges[i] = face.iedges[i]; }
    plane  = face.plane;
    for (i=0; i<3; i++) { rmin[i] = face.rmin[i]; rmax[i] = face.rmax[i]; }
    iold   = face.iold;
    inew   = face.inew;
    iprev  = face.iprev;
    inext  = face.inext;
    return *this;
  }

  void invert();
};

// ---------------------------------------------------- Global arrays ---
//G.Barrand : MacIntel : crash with g++-4.0.1 with -O on some subtract.
//            Anyway static of objects is proved to be not safe.
//            We put the below vector as members of BooleanProcessor.
//GB static std::vector<ExtNode> nodes;        // vector of nodes
//GB static std::vector<ExtEdge> edges;        // vector of edges
//GB static std::vector<ExtFace> faces;        // vector of faces

// ---------------------------------------------------- List of faces ---
class FaceList {
 private:
  std::vector<ExtFace>& faces; //G.Barrad : end
 private:
  int ihead;
  int ilast;

 public:
  //G.Barrand : FaceList() : ihead(0), ilast(0) {}
  FaceList(std::vector<ExtFace>& a_faces) : faces(a_faces),ihead(0),ilast(0) {}
  ~FaceList() {}

  void clean() { ihead = 0; ilast = 0; }
  int front()  { return ihead; }

  void push_back(int i) {
    if (ilast == 0) { ihead = i; } else { faces[ilast].inext = i; }
    ExtFace& face = faces[i]; //G.Barrand : optimize.
    face.iprev = ilast;
    face.inext = 0;
    ilast = i;
  }

  void remove(int i) {
    ExtFace& face = faces[i]; //G.Barrand : optimize.
    if (ihead == i) {
      ihead = face.inext;
    }else{
      faces[face.iprev].inext = face.inext;
    }
    if (ilast == i) {
      ilast = face.iprev;
    }else{
      faces[face.inext].iprev = face.iprev;
    }
    face.iprev = 0;
    face.inext = 0;
  }
};

// --------------------- Polyhedron with extended access to
//                       its members from the BooleanProcessor class ---
class ExtPolyhedron : public HepPolyhedron {
  friend class BooleanProcessor;
  virtual HepPolyhedron& operator = (const HepPolyhedron& from) {
    return HepPolyhedron::operator = (from);
  }
};

// ----------------------------------------- Boolean processor class ---
class BooleanProcessor {
 private:
  static int ishift; //G.Barrand
  std::vector<ExtNode> nodes;        // vector of nodes //G.Barrand
  std::vector<ExtEdge> edges;        // vector of edges //G.Barrand
  std::vector<ExtFace> faces;        // vector of faces //G.Barrand
 private:
  int             processor_error;   // is set in case of error
  int             operation;  // 0 (union), 1 (intersection), 2 (subtraction)
  int             ifaces1, ifaces2;  // lists of faces
  int             iout1,   iout2;    // lists of faces with status "out"
  int             iunk1,   iunk2;    // lists of faces with status "unknown"
  double          rmin[3], rmax[3];  // intersection of bounding boxes
  double          del;               // precision (tolerance)

  FaceList        result_faces;      // list of accepted faces
  FaceList        suitable_faces;    // list of suitable faces
  FaceList        unsuitable_faces;  // list of unsuitable faces
  FaceList        unknown_faces;     // list of unknown faces

  vector<int>     external_contours; // heads of external contours
  vector<int>     internal_contours; // heads of internal contours

 private:
  void   takePolyhedron(const HepPolyhedron & p, double, double, double);
  double findMinMax();
  void   selectOutsideFaces(int & ifaces, int & iout);
  int    testFaceVsPlane(ExtEdge & edge);
  void   renumberNodes(int & i1, int & i2, int & i3, int & i4);
  int    testEdgeVsEdge(ExtEdge & edge1, ExtEdge & edge2);
  void   removeJunkNodes() { while(nodes.back().s != 0) nodes.pop_back(); }
  void   divideEdge(int & i1, int & i2);
  void   insertEdge(const ExtEdge & edge);
  void   caseII(ExtEdge & edge1, ExtEdge & edge2);
  void   caseIE(ExtEdge & edge1, ExtEdge & edge2);
  void   caseEE(ExtEdge & edge1, ExtEdge & edge2);
  void   testFaceVsFace(int iface1, int iface2);
  void   invertNewEdges(int iface);
  void   checkDoubleEdges(int iface);
  void   assembleFace(int what, int iface);
  void   assembleNewFaces(int what, int ihead);
  void   initiateLists();
  void   assemblePolyhedra();
  void   findABC(double x1, double y1, double x2, double y2,
                 double &a, double &b, double &c) const;
  int    checkDirection(double *x, double *y) const;
  int    checkIntersection(int ix, int iy, int i1, int i2) const;
  void   mergeContours(int ix, int iy, int kext, int kint);
  int    checkTriangle(int iedge1, int iedge2, int ix, int iy) const;
  void   triangulateContour(int ix, int iy, int ihead);
  void   modifyReference(int iface, int i1, int i2, int iref);
  void   triangulateFace(int iface);
  HepPolyhedron createPolyhedron();

 public:
  //G.Barrand : BooleanProcessor() {}
  BooleanProcessor() //G.Barrand
  :processor_error(0)  // The next few fields are work space, initialised
  ,operation(0)        // here to prevent Coverity warnings.
  ,ifaces1(0)          // "
  ,ifaces2(0)          // "
  ,iout1(0)            // "
  ,iout2(0)            // "
  ,iunk1(0)            // "
  ,iunk2(0)            // "
  ,del(0.)             // "
  ,result_faces(faces)
  ,suitable_faces(faces)
  ,unsuitable_faces(faces)
  ,unknown_faces(faces)
  {  // rmin, rmax also initialised here to prevent Coverity warnings.
    for (int i = 0; i < 3; i++) {
      rmin[i] = 0.;
      rmax[i] = 0.;
    }
  }

  ~BooleanProcessor() {}

  HepPolyhedron execute(int op,
                        const HepPolyhedron &a,
                        const HepPolyhedron &b,
                        int& err);

  void draw();
  void draw_edge(int, int);
  void draw_contour(int, int, int);
  void draw_faces(int, int, int);
  void print_face(int);
  void print_edge(int);
  int get_processor_error() const {return processor_error;}

  void dump(); //G.Barrand
  static int get_shift(); //G.Barrand
  static void set_shift(int); //G.Barrand
  static int get_num_shift(); //G.Barrand
};

inline void ExtFace::invert()
/***********************************************************************
 *                                                                     *
 * Name: ExtFace::invert()                           Date:    28.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Invert face                                               *
 *                                                                     *
 ***********************************************************************/
{
  int iEprev, iEcur, iEnext;

  iEprev = 0; iEcur = iold;
  while (iEcur > 0) {
    ExtEdge& edge = edges[iEcur]; //G.Barrand : optimize.
    edge.invert();
    iEnext = edge.inext;
    edge.inext = iEprev;
    iEprev = iEcur;
    iEcur  = iEnext;
  }
  if (iold > 0) iold = iEprev;

  iEprev = 0; iEcur = inew;
  while (iEcur > 0) {
    ExtEdge& edge = edges[iEcur]; //G.Barrand : optimize.
    edge.invert();
    iEnext = edge.inext;
    edge.inext = iEprev;
    iEprev = iEcur;
    iEcur  = iEnext;
  }
  if (inew > 0) inew = iEprev;

#ifdef BP_GEANT4 //G.Barrand
  plane = HVPlane3D(-plane.a(), -plane.b(), -plane.c(), -plane.d());
#else
  plane = HVPlane3D(-plane.getNormal(), -plane.getDistanceFromOrigin());
#endif
}

void BooleanProcessor::takePolyhedron(const HepPolyhedron & p,
                                      double dx, double dy, double dz)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::takePolyhedron            Date:    16.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Transfer Polyhedron to internal representation            *
 *                                                                     *
 ***********************************************************************/
{
  int i, k, nnode, iNodes[5], iVis[4], iFaces[4];
  int dnode = nodes.size() - 1;
  int dface = faces.size() - 1;

  //   S E T   N O D E S

  //  for (i=1; i <= p.GetNoVertices(); i++) {
  //    nodes.push_back(ExtNode(p.GetVertex(i)));
  //  }

  HVPoint3D ppp;
  for (i=1; i <= p.GetNoVertices(); i++) {
#ifdef BP_GEANT4 //G.Barrand
    ppp = p.GetVertex(i);
    ppp.setX(ppp.x()+dx);
    ppp.setY(ppp.y()+dy);
    ppp.setZ(ppp.z()+dz);
#else
    ppp = p.GetVertexFast(i);
    ppp += HVPoint3D(dx,dy,dz);
#endif
    nodes.push_back(ExtNode(ppp));
  }

  //   S E T   F A C E S

  for (int iface=1; iface <= p.GetNoFacets(); iface++) {
    faces.push_back(ExtFace(edges,edges.size()));

    //   S E T   F A C E   N O D E S

    p.GetFacet(iface, nnode, iNodes, iVis, iFaces);
    for (i=0; i<nnode; i++) {
      //if (iNodes[i] < 1 || iNodes[i] > p.GetNoVertices()) processor_error = 1;
      //if (iFaces[i] < 1 || iFaces[i] > p.GetNoFacets())   processor_error = 1;

      if (iNodes[i] < 1 || iNodes[i] > p.GetNoVertices()) { //G.Barrand
        processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::takePolyhedron : problem 1."
          << G4endl;
#endif
      }
      if (iFaces[i] < 1 || iFaces[i] > p.GetNoFacets()) { //G.Barrand
        processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::takePolyhedron : problem 2."
          << G4endl;
#endif
      }
      iNodes[i] += dnode;
      iFaces[i] += dface;
    }

    //   S E T   E D G E S

    iNodes[nnode] = iNodes[0];
    faces.back().iedges[3] = 0;
    for (i=0; i<nnode; i++) {
      faces.back().iedges[i] = edges.size();
      edges.push_back(ExtEdge(iNodes[i], iNodes[i+1],
                              iface+dface, iFaces[i], iVis[i]));
      edges.back().inext     = edges.size();
    }
    edges.back().inext = 0;

    //   S E T   F A C E   M I N - M A X

    ExtFace& face = faces.back();     //G.Barrand : optimize.
    for (i=0; i<3; i++) {
      face.rmin[i] = nodes[iNodes[0]].v[i];
      face.rmax[i] = nodes[iNodes[0]].v[i];
    }
    for (i=1; i<nnode; i++) {
      ExtNode& node = nodes[iNodes[i]]; //G.Barrand : optimize.
      for (k=0; k<3; k++) {
        if (face.rmin[k] > node.v[k])
            face.rmin[k] = node.v[k];
        if (face.rmax[k] < node.v[k])
            face.rmax[k] = node.v[k];
      }
    }

    //   S E T   F A C E   P L A N E

    HVNormal3D n = (nodes[iNodes[2]].v-nodes[iNodes[0]].v).cross
                        (nodes[iNodes[3]].v-nodes[iNodes[1]].v);
    HVPoint3D  point(0,0,0);

    for (i=0; i<nnode; i++) { point += nodes[iNodes[i]].v; }
    if (nnode > 1) point *= (1./nnode);


    //G.Barrand : faces.back().plane = HVPlane3D(n.unit(), point);
    faces.back().plane = HVPlane3D(n, point); //G.Barrand

    //   S E T   R E F E R E N C E   T O   T H E   N E X T   F A C E

    faces.back().inext = faces.size();
  }
  faces.back().inext = 0;
}

double BooleanProcessor::findMinMax()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::findMinMax                Date:    16.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find min-max (bounding) boxes for polyhedra               *
 *                                                                     *
 ***********************************************************************/
{
  if (ifaces1 == 0 || ifaces2 == 0) return 0;

  int    i, iface;
  double rmin1[3], rmax1[3];
  double rmin2[3], rmax2[3];

  //   F I N D   B O U N D I N G   B O X E S

  for (i=0; i<3; i++) {
    rmin1[i] = faces[ifaces1].rmin[i];
    rmax1[i] = faces[ifaces1].rmax[i];
    rmin2[i] = faces[ifaces2].rmin[i];
    rmax2[i] = faces[ifaces2].rmax[i];
  }

  iface = faces[ifaces1].inext;
  while(iface > 0) {
    ExtFace& face = faces[iface]; //G.Barrand
    for (i=0; i<3; i++) {
      if (rmin1[i] > face.rmin[i]) rmin1[i] = face.rmin[i];
      if (rmax1[i] < face.rmax[i]) rmax1[i] = face.rmax[i];
    }
    iface = face.inext;
  }

  iface = faces[ifaces2].inext;
  while(iface > 0) {
    ExtFace& face = faces[iface]; //G.Barrand
    for (i=0; i<3; i++) {
      if (rmin2[i] > face.rmin[i]) rmin2[i] = face.rmin[i];
      if (rmax2[i] < face.rmax[i]) rmax2[i] = face.rmax[i];
    }
    iface = face.inext;
  }

  //   F I N D   I N T E R S E C T I O N   O F   B O U N D I N G   B O X E S

  for (i=0; i<3; i++) {
    rmin[i] = (rmin1[i] > rmin2[i]) ? rmin1[i] : rmin2[i];
    rmax[i] = (rmax1[i] < rmax2[i]) ? rmax1[i] : rmax2[i];
  }

  //   F I N D   T O L E R A N C E

  double del1 = 0;
  double del2 = 0;
  for (i=0; i<3; i++) {
    if ((rmax1[i]-rmin1[i]) > del1) del1 = rmax1[i]-rmin1[i];
    if ((rmax2[i]-rmin2[i]) > del2) del2 = rmax2[i]-rmin2[i];
  }
  return ((del1 < del2) ? del1 : del2) / GRANULARITY;
}

void BooleanProcessor::selectOutsideFaces(int & ifaces, int & iout)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::selectOutsideFaces        Date:    10.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Preselection of outside faces                             *
 *                                                                     *
 ***********************************************************************/
{
  int i, outflag, iface = ifaces, *prev;
  HVPoint3D mmbox[8] = { HVPoint3D(rmin[0],rmin[1],rmin[2]),
                               HVPoint3D(rmax[0],rmin[1],rmin[2]),
                               HVPoint3D(rmin[0],rmax[1],rmin[2]),
                               HVPoint3D(rmax[0],rmax[1],rmin[2]),
                               HVPoint3D(rmin[0],rmin[1],rmax[2]),
                               HVPoint3D(rmax[0],rmin[1],rmax[2]),
                               HVPoint3D(rmin[0],rmax[1],rmax[2]),
                               HVPoint3D(rmax[0],rmax[1],rmax[2]) };
  prev = &ifaces;
  while (iface > 0) {

    //   B O U N D I N G   B O X   vs  B O U N D I N G   B O X

    outflag = 0;
    ExtFace& face = faces[iface]; //G.Barrand : optimize.
    for (i=0; i<3; i++) {
      if (face.rmin[i] > rmax[i] + del) { outflag = 1; break; }
      if (face.rmax[i] < rmin[i] - del) { outflag = 1; break; }
    }

    //   B O U N D I N G   B O X   vs  P L A N E

    if (outflag == 0) {
      int npos = 0, nneg = 0;
      double d;
      for (i=0; i<8; i++) {
        d = face.plane.distance(mmbox[i]); //G.Barrand : optimize
        if (d > +del) npos++;
        if (d < -del) nneg++;
      }
      if (npos == 8 || nneg == 8) outflag = 1;
    }

    //   U P D A T E   L I S T S

    if (outflag == 1) {
      *prev = face.inext;
      face.inext = iout;
      iout = iface;
    }else{
      prev = &face.inext;
    }

    iface = *prev;
  }
}

int BooleanProcessor::testFaceVsPlane(ExtEdge & edge)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::testFaceVsPlane           Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find result of intersection of face by plane              *
 *                                                                     *
 ***********************************************************************/
{
  int        iface = edge.iface1;
  HVPlane3D plane = faces[edge.iface2].plane;
  int        i, nnode, npos = 0, nneg = 0, nzer = 0;
  double     dd[5];

  //   F I N D   D I S T A N C E S

  nnode = (faces[iface].iedges[3] == 0) ? 3 : 4;
  for (i=0; i<nnode; i++) {
    dd[i] = plane.distance(nodes[edges[faces[iface].iedges[i]].i1].v);
    if (dd[i] > del) {
      npos++;
    }else if (dd[i] < -del) {
      nneg++;
    }else{
      nzer++; dd[i] = 0;
    }
  }

  //   S O M E   S I M P L E   C A S E S  ( N O   I N T E R S E C T I O N )

  if (npos == nnode || nneg == nnode)   return OUT_OF_PLANE;
  if (nzer == 1     && nneg == 0)       return OUT_OF_PLANE;
  if (nzer == 1     && npos == 0)       return OUT_OF_PLANE;
  if (nzer == nnode)                    return ON_PLANE;
  if (nzer == 3)                        return NON_PLANAR_FACE;

  //   F I N D   I N T E R S E C T I O N

  int       ie1 = 0, ie2 = 0, s1 = 0, s2 = 0, status, nint = 0;
  enum      { PLUS_MINUS, MINUS_PLUS, ZERO_ZERO, ZERO_PLUS, ZERO_MINUS };

  dd[nnode] = dd[0];
  for (i=0; i<nnode; i++) {
    if (dd[i] > 0) {
      if (dd[i+1] >= 0) continue;
      status = PLUS_MINUS;
    }else if (dd[i] < 0) {
      if (dd[i+1] <= 0) continue;
      status = MINUS_PLUS;
    }else{
      status = ZERO_ZERO;
      if (dd[i+1] > 0) status = ZERO_PLUS;
      if (dd[i+1] < 0) status = ZERO_MINUS;
    }
    switch (nint) {
    case 0:
      ie1 = i; s1 = status; nint++; break;
    case 1:
      ie2 = i; s2 = status; nint++; break;
    default:
      return NON_PLANAR_FACE;
    }
  }
  if (nint != 2)                        return NON_PLANAR_FACE;

  //   F O R M   I N T E R S E C T I O N   S E G M E N T

  if (s1 != ZERO_ZERO && s2 != ZERO_ZERO) {
    if (s1 == s2)                       return NON_PLANAR_FACE;
    int     iedge, i1 = 0, i2 = 0, ii[2];
    double  d1 = 0., d2 = 0., d3 = 0.;
    ii[0] = ie1; ii[1] = ie2;
    for (i=0; i<2; i++) {
      iedge = faces[iface].iedges[ii[i]];
      while (iedge > 0) {
        i1 = edges[iedge].i1;
        i2 = edges[iedge].i2;

        d1 = plane.distance(nodes[i1].v);
        d2 = plane.distance(nodes[i2].v);
        if (d1 > del) {
          if (d2 < -del) { ii[i] = nodes.size(); break; } // +-
        }else if (d1 < -del) {
          if (d2 >  del) { ii[i] = nodes.size(); break; } // -+
        }else{
          ii[i] = i1; break;                              // 0+ or 0-
        }
        iedge = edges[iedge].inext;
      }
      if (ii[i] == (int)nodes.size()) {
        d3 = d2-d1; d1 = d1/d3; d2 = d2/d3;
        nodes.push_back(ExtNode(d2*nodes[i1].v-d1*nodes[i2].v, iedge));
      }
    }
    edge.inext = 0;
    if (s1 == MINUS_PLUS || s1 == ZERO_PLUS) {
      edge.i1 = ii[1];
      edge.i2 = ii[0];
    }else{
      edge.i1 = ii[0];
      edge.i2 = ii[1];
    }
    return INTERSECTION;
  }else{
    if (npos == nneg)                   return NON_PLANAR_FACE;
    edge.inext = (s1 == ZERO_ZERO) ? ie1+1 : ie2+1;
    if (s1 == ZERO_PLUS || s2 == ZERO_MINUS) {
      edge.i1 = edges[faces[iface].iedges[ie2]].i1;
      edge.i2 = edges[faces[iface].iedges[ie1]].i1;
    }else{
      edge.i1 = edges[faces[iface].iedges[ie1]].i1;
      edge.i2 = edges[faces[iface].iedges[ie2]].i1;
    }
    return EDGE;
  }
}

void BooleanProcessor::renumberNodes(int & i1, int & i2, int & i3, int & i4)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::renumberNodes             Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Renumber nodes and remove last temporary node.            *
 *           Remark: In principal this routine can be replaced just    *
 *           with i1 = i2;                                             *
 *           Removal of temporary nodes provides additional control    *
 *           on number of nodes, that is very useful for debugging.    *
 *                                                                     *
 ***********************************************************************/
{
  if (i1 == i2) return;
  if (nodes[i1].s == 0 || nodes.back().s == 0) { i1 = i2; return; }

  int ilast = nodes.size()-1;
  if (i1 == ilast) { i1 = i2; nodes.pop_back(); return; }
  if (i2 == ilast) { i2 = i1; }
  if (i3 == ilast) { i3 = i1; }
  if (i4 == ilast) { i4 = i1; }
  nodes[i1] = nodes.back(); i1 = i2; nodes.pop_back();
}

int BooleanProcessor::testEdgeVsEdge(ExtEdge & edge1, ExtEdge & edge2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::testEdgeVsEdge            Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find common part of two edges                             *
 *                                                                     *
 ***********************************************************************/
{
  int    i, ii = 0;
  double d, dd = 0.;

  for (i=0; i<3; i++) {
    d = nodes[edge1.i1].v[i]-nodes[edge1.i2].v[i];
    if (d < 0.) d = -d;
    if (d > dd) { dd = d; ii = i; }
  }
  double t1 = nodes[edge1.i1].v[ii];
  double t2 = nodes[edge1.i2].v[ii];
  double t3 = nodes[edge2.i1].v[ii];
  double t4 = nodes[edge2.i2].v[ii];
  if (t2-t1 < 0.) { t1 = -t1; t2 = -t2; t3 = -t3; t4 = -t4; }

  if (t3 <= t1+del || t4 >= t2-del) return 0;
  if (t3 > t2+del) {
    renumberNodes(edge2.i1, edge1.i2, edge1.i1, edge2.i2);
  }else if (t3 < t2-del) {
    renumberNodes(edge1.i2, edge2.i1, edge1.i1, edge2.i2);
  }
  if (t4 < t1-del) {
    renumberNodes(edge2.i2, edge1.i1, edge1.i2, edge2.i1);
  }else if (t4 > t1+del) {
    renumberNodes(edge1.i1, edge2.i2, edge1.i2, edge2.i1);
  }
  return 1;
}

void BooleanProcessor::divideEdge(int & i1, int & i2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::divideEdge                Date:    24.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Unify the nodes and divide edge on two parts by the node. *
 *                                                                     *
 ***********************************************************************/
{
  int iedges[2];
  iedges[0] = nodes[i1].s;
  iedges[1] = nodes[i2].s;

  //   U N I F Y   N O D E S

  if      (i1 < i2) { i2 = i1; }
  else if (i1 > i2) { i1 = i2; }
  else              { iedges[1] = 0; }
  if (iedges[0] == iedges[1]) return;

  int ie1, ie2, inode = i1;
  nodes[inode].s = 0;
  for (int i=0; i<2; i++) {

    //   F I N D   C O R R E S P O N D I N G   E D G E

    if ((ie1 = iedges[i]) == 0) continue;
    ie2 = faces[edges[ie1].iface2].iedges[0];
    while (ie2 > 0) {
      if (edges[ie2].i1 == edges[ie1].i2 &&
          edges[ie2].i2 == edges[ie1].i1) break;
      ie2 = edges[ie2].inext;
    }

    //   D I V I D E   E D G E S

    edges.push_back(edges[ie1]);
    edges[ie1].inext = edges.size() - 1;
    edges[ie1].i2    = inode;
    edges.back().i1  = inode;

    edges.push_back(edges[ie2]);
    edges[ie2].inext = edges.size() - 1;
    edges[ie2].i2    = inode;
    edges.back().i1  = inode;
  }
}

void BooleanProcessor::insertEdge(const ExtEdge & edge)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::insertEdge                Date:    24.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Insert edge to the list of new edges                      *
 *                                                                     *
 ***********************************************************************/
{
  int iface = edge.iface1;
  edges.push_back(edge);
  edges.back().inext = faces[iface].inew;
  faces[iface].inew  = edges.size() - 1;
}

void BooleanProcessor::caseII(ExtEdge & edge1, ExtEdge & edge2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::caseII                    Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Intersection/Intersection case                            *
 *                                                                     *
 ***********************************************************************/
{
  divideEdge(edge1.i1, edge2.i2);
  divideEdge(edge1.i2, edge2.i1);
  edge1.ivis = 1;
  edge2.ivis = 1;
  insertEdge(edge1);
  insertEdge(edge2);
}

void BooleanProcessor::caseIE(ExtEdge &, ExtEdge &)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::caseIE                    Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Intersection/Edge-touch case                              *
 *                                                                     *
 ***********************************************************************/
{
  processor_error = 1;
#ifdef BP_DEBUG
  G4cout
    << "BooleanProcessor::caseIE : unimplemented case"
    << G4endl;
#endif
}

void BooleanProcessor::caseEE(ExtEdge &, ExtEdge &)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::caseEE                    Date:    19.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Edge-touch/Edge-touch case                                *
 *                                                                     *
 ***********************************************************************/
{
  processor_error = 1;
#ifdef BP_DEBUG
  G4cout
    << "BooleanProcessor::caseEE : unimplemented case"
    << G4endl;
#endif
}

void BooleanProcessor::testFaceVsFace(int iface1, int iface2)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::testFaceVsFace            Date:    11.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find result (an edge) of intersection of two faces        *
 *                                                                     *
 ***********************************************************************/
{
  ExtEdge edge1, edge2;
  int     irep1, irep2;

  //   M I N - M A X

 {const ExtFace& face_1 = faces[iface1]; //G.Barrand : optimize
  const ExtFace& face_2 = faces[iface2];
  for (int i=0; i<3; i++) {
    if (face_1.rmin[i] > face_2.rmax[i] + del) return;
    if (face_1.rmax[i] < face_2.rmin[i] - del) return;
  }}

  //   F A C E - 1   vs   P L A N E - 2

  edge1.iface1 = iface1;
  edge1.iface2 = iface2;
  irep1        = testFaceVsPlane(edge1);
  if (irep1 == OUT_OF_PLANE || irep1 == ON_PLANE) {
    removeJunkNodes();
    return;
  }

  //   F A C E - 2   vs   P L A N E - 1

  edge2.iface1 = iface2;
  edge2.iface2 = iface1;
  irep2        = testFaceVsPlane(edge2);
  if (irep2 == OUT_OF_PLANE || irep2 == ON_PLANE) {
    removeJunkNodes();
    return;
  }

  //   C H E C K   F O R   N O N P L A N A R   F A C E

  if (irep1 == NON_PLANAR_FACE || irep2 == NON_PLANAR_FACE) {
    removeJunkNodes();
    return;
  }

  //   F I N D   I N T E R S E C T I O N   P A R T

  if (testEdgeVsEdge(edge1, edge2) == 0) return;

  //   C O N S I D E R   D I F F E R E N T   C A S E S

  if (irep1 == INTERSECTION && irep2 == INTERSECTION) caseII(edge1, edge2);
  if (irep1 == INTERSECTION && irep2 == EDGE)         caseIE(edge1, edge2);
  if (irep1 == EDGE         && irep2 == INTERSECTION) caseIE(edge2, edge1);
  if (irep1 == EDGE         && irep2 == EDGE)         caseEE(edge1, edge2);
  removeJunkNodes();

}

void BooleanProcessor::invertNewEdges(int iface)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::invertNewEdges            Date:    04.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Invert direction of new edges                             *
 *                                                                     *
 ***********************************************************************/
{
  int iedge = faces[iface].inew;
  while (iedge > 0) {
    edges[iedge].invert();
    iedge = edges[iedge].inext;
  }
}

void BooleanProcessor::checkDoubleEdges(int)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkDoubleEdges          Date:    04.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Eliminate duplication of edges                            *
 *                                                                     *
 ***********************************************************************/
{

}

void BooleanProcessor::assembleFace(int what, int iface)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::assembleFace              Date:    19.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Assemble face                                             *
 *                                                                     *
 ***********************************************************************/
{
  //   A S S E M B L E   N E W   F A C E

  int ihead;      // head of the list of edges for new face
  int icur;       // current edge in the list - last edge inserted to the list
  int *ilink;     // pointer to the current link
  int ifirst;     // first node of a contour
  int *i;         // pointer to the index of the current edge in a loop
  int ioldflag=0; // is set if an edge from iold has been taken

#define INSERT_EDGE_TO_THE_LIST(A) \
*ilink = A; ilink = &edges[A].inext; *ilink = 0

  ExtFace& face = faces[iface]; //G.Barrand : optimize.
  ilink = &ihead;
  for(;;) {
    if (face.inew == 0) break;

    //   S T A R T   N E W   C O N T O U R

    icur   = face.inew;
    face.inew = edges[icur].inext;
    INSERT_EDGE_TO_THE_LIST(icur);
    ifirst = edges[icur].i1;

    //   C O N S T R U C T   T H E   C O N T O U R

    for (;;) {
      i = &face.inew;
      ExtEdge& edge_cur = edges[icur];
      while(*i > 0) {
        ExtEdge& edge_i = edges[*i];
        if (edge_i.i1 == edge_cur.i2) break;
        i = &edge_i.inext;
      }
      if (*i == 0) {
        i = &face.iold;
        while(*i > 0) {
          ExtEdge& edge_i = edges[*i];
          if (edge_i.i1 == edge_cur.i2) {
            ioldflag = 1;
            break;
          }
          i = &edge_i.inext;
        }
      }
      if (*i > 0) {
        icur = *i;
        *i = edges[icur].inext;
        INSERT_EDGE_TO_THE_LIST(icur);
        if (edges[icur].i2 == ifirst) { break; } else { continue; }
      }else{
        processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::assembleFace(" << iface << ") : "
          << "could not find next edge of the contour"
          << G4endl;
#endif
        face.inew = DEFECTIVE_FACE;
        return;
      }
    }
  }

  //   C H E C K   O R I G I N A L   C O N T O U R

  int iedge;
  iedge = face.iold;
  if (what == 0 && ioldflag == 0 && iedge > 0) {
    for (;;) {
      if (edges[iedge].inext > 0) {
        if (edges[iedge].i2 == edges[edges[iedge].inext].i1) {
          iedge = edges[iedge].inext;
        }else{
          break;
        }
      }else{
        if (edges[iedge].i2 == edges[face.iold].i1) {
          edges[iedge].inext = ihead;   // set new face
          return;
        }else{
          break;
        }
      }
    }
  }

  //   M A R K   U N S U I T A B L E   N E I G H B O U R I N G   F A C E S

  int iface2;
  iedge = face.iold;
  while(iedge > 0) {
    iface2 = edges[iedge].iface2;
    if (faces[iface2].inew == 0) faces[iface2].inew = UNSUITABLE_FACE;
    iedge = edges[iedge].inext;
  }
  face.iold = ihead;            // set new face
}

void BooleanProcessor::assembleNewFaces(int what, int ihead)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::assembleNewFaces          Date:    30.01.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Assemble internal or external parts of faces              *
 *                                                                     *
 ***********************************************************************/
{
  int iface = ihead;
  while(iface > 0) {
    if (faces[iface].inew > 0) {
      if (what != 0) invertNewEdges(iface);
      checkDoubleEdges(iface);
      assembleFace(what, iface);
      faces[iface].inew =
        (faces[iface].iold == 0) ? UNSUITABLE_FACE : NEW_FACE;
    }
    iface = faces[iface].inext;
  }
}

void BooleanProcessor::initiateLists()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::initiateLists             Date:    28.02.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Initiate lists of faces.                                  *
 *                                                                     *
 ***********************************************************************/
{
  int i, iface;

  //   R E S E T   L I S T S   O F   F A C E S

  result_faces.clean();
  suitable_faces.clean();
  unsuitable_faces.clean();
  unknown_faces.clean();

  //   I N I T I A T E   T H E   L I S T S

  iface = iout1;
  while (iface > 0) {
    i     = iface;
    iface = faces[i].inext;
    if (operation == OP_INTERSECTION) {
      unsuitable_faces.push_back(i);
      faces[i].inew = UNSUITABLE_FACE;
    }else{
      suitable_faces.push_back(i);
      faces[i].inew = ORIGINAL_FACE;
    }
  }
  iface = iout2;
  while (iface > 0) {
    i     = iface;
    iface = faces[i].inext;
    if (operation == OP_UNION) {
      suitable_faces.push_back(i);
      faces[i].inew = ORIGINAL_FACE;
    }else{
      unsuitable_faces.push_back(i);
      faces[i].inew = UNSUITABLE_FACE;
    }
  }

  iface = iunk1;
  while (iface > 0) {
    i     = iface;
    iface = faces[i].inext;
    unknown_faces.push_back(i);
  }
  iface = iunk2;
  while (iface > 0) {
    i     = iface;
    iface = faces[i].inext;
    if (operation == OP_SUBTRACTION) faces[i].invert();
    unknown_faces.push_back(i);
  }

  iface = ifaces1;
  while (iface > 0) {
    i     = iface;
    iface = faces[i].inext;
    switch(faces[i].inew) {
    case UNKNOWN_FACE:
      unknown_faces.push_back(i);
      break;
    case ORIGINAL_FACE: case NEW_FACE:
      suitable_faces.push_back(i);
      break;
    case UNSUITABLE_FACE:
      unsuitable_faces.push_back(i);
      break;
    default:
      faces[i].iprev = 0;
      faces[i].inext = 0;
      break;
    }
  }
  iface = ifaces2;
  while (iface > 0) {
    i     = iface;
    iface = faces[i].inext;
    if (operation == OP_SUBTRACTION) faces[i].invert();
    switch(faces[i].inew) {
    case UNKNOWN_FACE:
      unknown_faces.push_back(i);
      break;
    case ORIGINAL_FACE: case NEW_FACE:
      suitable_faces.push_back(i);
      break;
    case UNSUITABLE_FACE:
      unsuitable_faces.push_back(i);
      break;
    default:
      faces[i].iprev = 0;
      faces[i].inext = 0;
      break;
    }
  }
  ifaces1 = ifaces2 = iout1 = iout2 = iunk1 = iunk2 = 0;
}

void BooleanProcessor::assemblePolyhedra()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::assemblePolyhedra()       Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Collect suitable faces and remove unsuitable ones.        *
 *                                                                     *
 ***********************************************************************/
{
  int i, iedge, iface;

  //   L O O P   A L O N G   S U I T A B L E   F A C E S

  iface = suitable_faces.front();
  while(iface > 0) {
    i = iface;
    iedge = faces[i].iold;
    while(iedge > 0) {
      iface = edges[iedge].iface2;
      if (faces[iface].inew == UNKNOWN_FACE) {
        unknown_faces.remove(iface);
        suitable_faces.push_back(iface);
        faces[iface].inew = ORIGINAL_FACE;
      }
      iedge = edges[iedge].inext;
    }
    iface = faces[i].inext;
    suitable_faces.remove(i);
    result_faces.push_back(i);
  }
  if (unknown_faces.front() == 0) return;

  //   L O O P   A L O N G   U N S U I T A B L E   F A C E S

  iface = unsuitable_faces.front();
  while(iface > 0) {
    i = iface;
    iedge = faces[i].iold;
    while(iedge > 0) {
      iface = edges[iedge].iface2;
      if (faces[iface].inew == UNKNOWN_FACE) {
        unknown_faces.remove(iface);
        unsuitable_faces.push_back(iface);
        faces[iface].inew = UNSUITABLE_FACE;
      }
      iedge = edges[iedge].inext;
    }
    iface = faces[i].inext;
    unsuitable_faces.remove(i);
  }

  //G.Barrand : begin
  /* From S.Ponce
   At last, there is a problem in the assemblePolyhedra method. At least, I
  think it is there. The problem deals with boolean operations on solids,
  when one of the two contains entirely the other one. It has no sense for
  intersection and union but still has sense for subtraction. In this
  case, faces from the inner solid are stored in the unknown_faces
  FaceList. And an error occurs in the execute method. This may be correct
  for intersection and union but in the case of subtraction, one should do
  that in assemblePolyhedra :
  */
  //   Unknown faces are actually suitable face !!!
   iface = unknown_faces.front();
   while(iface > 0) {
     i = iface;
     faces[i].inew = ORIGINAL_FACE;
     iface = faces[i].inext;
     unknown_faces.remove(i);
     result_faces.push_back(i);
   }
  /*
   Otherwise, the inner hole that the second solid was building in the
  first one does not exist. I'm not very clear on what to do for unions
  and intersections. I think this kind of situation should be detected and
  one of the solid should simply be ignored.
  */
  //G.Barrand : end
}

inline void
BooleanProcessor::findABC(double x1, double y1, double x2, double y2,
                          double &a, double &b, double &c) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::findABC                   Date:    07.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Find line equation Ax+By+C=0                              *
 *                                                                     *
 ***********************************************************************/
{
  double w;
  a  = y1 - y2;
  b  = x2 - x1;
  //G.Barrand : w  = std::abs(a)+std::abs(b);
  w  = ::fabs(a)+::fabs(b); //G.Barrand
  a /= w;
  b /= w;
  c  = -(a*x2 + b*y2);
}

int BooleanProcessor::checkDirection(double *x, double *y) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkDirection            Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Check direction of line 1-4                               *
 *                                                                     *
 ***********************************************************************/
{
  double a1, b1, c1, a2, b2, c2, d1, d2;

  //   T E S T   L I N E   1 - 4   V S   E X T E R N A L   C O N T O U R

  findABC(x[0], y[0], x[1], y[1], a1, b1, c1);
  findABC(x[1], y[1], x[2], y[2], a2, b2, c2);
  d1 = a1*x[4] + b1*y[4] + c1;
  d2 = a2*x[4] + b2*y[4] + c2;
  if (d1 <= del && d2 <= del)            return 1;
  if (! (d1 > del && d2 > del)) {
    if ( a1*x[2] + b1*y[2] + c1 >= -del) return 1;
  }

  //   T E S T   L I N E   1 - 4   V S   I N T E R N A L   C O N T O U R

  findABC(x[3], y[3], x[4], y[4], a1, b1, c1);
  findABC(x[4], y[4], x[5], y[5], a2, b2, c2);
  d1 = a1*x[1] + b1*y[1] + c1;
  d2 = a2*x[1] + b2*y[1] + c2;
  if (d1 <= del && d2 <= del)            return 1;
  if (!(d1 > del && d2 > del)) {
    if ( a1*x[5] + b1*y[5] + c1 >= -del) return 1;
  }
  return 0;
}

int BooleanProcessor::checkIntersection(int ix, int iy, int i1, int i2) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkDirection            Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Check line i1-i2 on intersection with contours            *
 *                                                                     *
 ***********************************************************************/
{
  //  F I N D   L I N E   E Q U A T I O N

  double x1, y1, x2, y2, a1, b1, c1;
  x1 = nodes[i1].v[ix];
  y1 = nodes[i1].v[iy];
  x2 = nodes[i2].v[ix];
  y2 = nodes[i2].v[iy];
  findABC(x1, y1, x2, y2, a1, b1, c1);

  //  L O O P   A L O N G   E X T E R N A L   C O N T O U R S

  int icontour, iedge, k1, k2;
  double x3, y3, x4, y4, a2, b2, c2, d1, d2;
  for(icontour=0; icontour<(int)external_contours.size(); icontour++) {
    iedge = external_contours[icontour];
    while(iedge > 0) {
      k1 = edges[iedge].i1;
      k2 = edges[iedge].i2;
      iedge = edges[iedge].inext;
      if (k1 == i1 || k2 == i1) continue;
      if (k1 == i2 || k2 == i2) continue;
      x3 = nodes[k1].v[ix];
      y3 = nodes[k1].v[iy];
      x4 = nodes[k2].v[ix];
      y4 = nodes[k2].v[iy];
      d1 = a1*x3 + b1*y3 + c1;
      d2 = a1*x4 + b1*y4 + c1;
      if (d1 >  del && d2 >  del) continue;
      if (d1 < -del && d2 < -del) continue;

      findABC(x3, y3, x4, y4, a2, b2, c2);
      d1 = a2*x1 + b2*y1 + c2;
      d2 = a2*x2 + b2*y2 + c2;
      if (d1 >  del && d2 >  del) continue;
      if (d1 < -del && d2 < -del) continue;
      return 1;
    }
  }

  //  L O O P   A L O N G   E X T E R N A L   C O N T O U R S

  for(icontour=0; icontour<(int)internal_contours.size(); icontour++) {
    iedge = internal_contours[icontour];
    while(iedge > 0) {
      k1 = edges[iedge].i1;
      k2 = edges[iedge].i2;
      iedge = edges[iedge].inext;
      if (k1 == i1 || k2 == i1) continue;
      if (k1 == i2 || k2 == i2) continue;
      x3 = nodes[k1].v[ix];
      y3 = nodes[k1].v[iy];
      x4 = nodes[k2].v[ix];
      y4 = nodes[k2].v[iy];
      d1 = a1*x3 + b1*y3 + c1;
      d2 = a1*x4 + b1*y4 + c1;
      if (d1 >  del && d2 >  del) continue;
      if (d1 < -del && d2 < -del) continue;

      findABC(x3, y3, x4, y4, a2, b2, c2);
      d1 = a2*x1 + b2*y1 + c2;
      d2 = a2*x2 + b2*y2 + c2;
      if (d1 >  del && d2 >  del) continue;
      if (d1 < -del && d2 < -del) continue;
      return 1;
    }
  }
  return 0;
}

void BooleanProcessor::mergeContours(int ix, int iy, int kext, int kint)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::mergeContours             Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Attemp to merge internal contour with external one        *
 *                                                                     *
 ***********************************************************************/
{
  int    i1ext, i2ext, i1int, i2int, i, k[6];
  double x[6], y[6];

  //   L O O P   A L O N G   E X T E R N A L   C O N T O U R

  i1ext = external_contours[kext];
  while (i1ext > 0) {
    i2ext = edges[i1ext].inext;
    if (i2ext == 0) i2ext = external_contours[kext];
    k[0] = edges[i1ext].i1;
    k[1] = edges[i1ext].i2;
    k[2] = edges[i2ext].i2;
    for (i=0; i<3; i++) {
      x[i] = nodes[k[i]].v[ix];
      y[i] = nodes[k[i]].v[iy];
    }

    //   L O O P   A L O N G   I N T E R N A L   C O N T O U R

    i1int = internal_contours[kint];
    while (i1int > 0) {
      i2int = edges[i1int].inext;
      if (i2int == 0) i2int = internal_contours[kint];
      k[3] = edges[i1int].i1;
      k[4] = edges[i1int].i2;
      k[5] = edges[i2int].i2;
      for (i=3; i<6; i++) {
        x[i] = nodes[k[i]].v[ix];
        y[i] = nodes[k[i]].v[iy];
      }

      //   T E S T   L I N E   K1 - K4
      //   I F   O K   T H E N   M E R G E   C O N T O U R S

      if (checkDirection(x, y) == 0) {
        if (checkIntersection(ix, iy, k[1], k[4]) == 0) {
          i = i1int;
          for(;;) {
            if (edges[i].inext == 0) {
              edges[i].inext = internal_contours[kint];
              internal_contours[kint] = 0;
              break;
            }else{
              i = edges[i].inext;
            }
          }
          i = edges[i1int].iface1;
          edges.push_back(ExtEdge(k[1], k[4], i, -(int(edges.size())+1), -1));
          edges.back().inext = i2int;
          edges.push_back(ExtEdge(k[4], k[1], i, -(int(edges.size())-1), -1));
          edges.back().inext = edges[i1ext].inext;
          edges[i1ext].inext = edges.size()-2;
          edges[i1int].inext = edges.size()-1;
          return;
        }
      }
      i1int = edges[i1int].inext;
    }
    i1ext = edges[i1ext].inext;
  }
}

int BooleanProcessor::checkTriangle(int iedge1, int iedge2, int ix, int iy) const
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::checkTriangle             Date:    08.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Check triangle for correctness                            *
 *                                                                     *
 ***********************************************************************/
{
  int    k[3];
  double x[3], y[3];
  double a1, b1, c1;

  k[0] = edges[iedge1].i1;
  k[1] = edges[iedge1].i2;
  k[2] = edges[iedge2].i2;
  for (int i=0; i<3; i++) {
    x[i] = nodes[k[i]].v[ix];
    y[i] = nodes[k[i]].v[iy];
  }

  //  C H E C K   P R I N C I P A L   C O R R E C T N E S S

  findABC(x[2], y[2], x[0], y[0], a1, b1, c1);
  if (a1*x[1]+b1*y[1]+c1 <= 0.1*del) return 1;

  //   C H E C K   T H A T   T H E R E   I S   N O   P O I N T S   I N S I D E

  int    inode, iedge;
  double a2, b2, c2, a3, b3, c3;

  findABC(x[0], y[0], x[1], y[1], a2, b2, c2);
  findABC(x[1], y[1], x[2], y[2], a3, b3, c3);
  iedge = iedge2;
  for (;;) {
    iedge = edges[iedge].inext;
    if (edges[iedge].inext == iedge1) return 0;
    inode = edges[iedge].i2;
    if (inode == k[0])                continue;
    if (inode == k[1])                continue;
    if (inode == k[2])                continue;
    x[1]  = nodes[inode].v[ix];
    y[1]  = nodes[inode].v[iy];
    if (a1*x[1]+b1*y[1]+c1 < -0.1*del)    continue;
    if (a2*x[1]+b2*y[1]+c2 < -0.1*del)    continue;
    if (a3*x[1]+b3*y[1]+c3 < -0.1*del)    continue;
    return 1;
  }
  return 0; // default return
}

void BooleanProcessor::triangulateContour(int ix, int iy, int ihead)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::triangulateContour        Date:    06.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Triangulate external contour                              *
 *                                                                     *
 ***********************************************************************/
{

  //G4cout << "Next Countour" << G4endl;
  //int draw_flag = 0;
  //if (draw_flag) draw_contour(5, 3, ihead);

  //   C L O S E   C O N T O U R

  int ipnext = ihead, nnode = 1;
  for (;;) {
    if (edges[ipnext].inext > 0) {
      ipnext = edges[ipnext].inext;
      nnode++;
    }else{
      edges[ipnext].inext = ihead;
      break;
    }
  }

  //   L O O P   A L O N G   C O N T O U R

  //G4cerr << "debug : contour : begin : =================" << G4endl;
  //dump();//debug

  int iedge1, iedge2, iedge3, istart = 0;
  for (;;) {
    iedge1 = edges[ipnext].inext;
    iedge2 = edges[iedge1].inext;
/*
    G4cerr << "debug :"
              << " ipnext " << ipnext
              << " iedge1 " << iedge1
              << " iedge2 " << iedge2
              << " : istart " << istart
              << G4endl;
*/
    if (istart == 0) {
      istart = iedge1;
      if (nnode <= 3) {
        iedge3 = edges[iedge2].inext;
        edges[iedge1].iface1 = faces.size();
        edges[iedge2].iface1 = faces.size();
        edges[iedge3].iface1 = faces.size();
        edges[iedge3].inext = 0;
        faces.push_back(ExtFace(edges,0)); //G.Barrand : ok ?
        faces.back().iold = iedge1;
        faces.back().inew = ORIGINAL_FACE;

  //if (draw_flag) draw_contour(4, 2, iedge1);

        break;
      }
    }else if (istart == iedge1) {
      processor_error = 1;
#ifdef BP_DEBUG
      G4cerr
        << "BooleanProcessor::triangulateContour : "
        << "could not generate a triangle (infinite loop)"
        << G4endl;
#endif
      break;
    }

    //   C H E C K   C O R E C T N E S S   O F   T H E   T R I A N G L E

    if (checkTriangle(iedge1,iedge2,ix,iy) != 0) {
      ipnext  = edges[ipnext].inext;
      continue;
    }

    //   M O D I F Y   C O N T O U R

    int i1 = edges[iedge1].i1;
    int i3 = edges[iedge2].i2;
    int iface1 = edges[iedge1].iface1;
    int iface2 = faces.size();

    edges[ipnext].inext = edges.size();
    edges.push_back(ExtEdge(i1, i3, iface1, -(int(edges.size())+1), -1));
    edges.back().inext = edges[iedge2].inext;

    //   A D D   N E W   T R I A N G L E   T O   T H E   L I S T

    edges[iedge2].inext = edges.size();
    edges.push_back(ExtEdge(i3, i1, iface2, -(int(edges.size())-1), -1));
    faces.push_back(ExtFace(edges,0)); //G.Barrand : ok ?
    faces.back().iold   = iedge1;
    faces.back().inew   = ORIGINAL_FACE;
    edges[iedge1].iface1 = iface2;
    edges[iedge2].iface1 = iface2;
    ipnext  = edges[ipnext].inext;
    istart = 0;
    nnode--;

  //if (draw_flag)  draw_contour(4, 2, iedge1);

  }
}

void BooleanProcessor::modifyReference(int iface, int i1, int i2, int iref)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::modifyReference           Date:    13.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Modify reference to the neighbouring face                 *
 *                                                                     *
 ***********************************************************************/
{
  int iedge = faces[iface].iold;
  while (iedge > 0) {
    if (edges[iedge].i1 == i2 && edges[iedge].i2 == i1) {
      edges[iedge].iface2 = iref;
      return;
    }
    iedge = edges[iedge].inext;
  }
  processor_error = 1;
#ifdef BP_DEBUG
  G4cerr
    << "BooleanProcessor::modifyReference : could not find the edge, "
    << "iface=" << iface << ", i1,i2=" << i1 << "," << i2 << ", iref=" << iref
    << G4endl;
#endif
}

void BooleanProcessor::triangulateFace(int iface)
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::triangulateFace           Date:    02.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Triangulation of an extended face                         *
 *                                                                     *
 ***********************************************************************/
{

  //   F I N D   M A X   C O M P O N E N T   O F   T H E   N O R M A L
  //   S E T  IX, IY, IZ

#ifdef BP_GEANT4 //G.Barrand
  HVNormal3D normal = faces[iface].plane.normal();
#else
  const HVNormal3D& normal = faces[iface].plane.getNormal();
#endif
  int ix, iy, iz = 0;
  //G.Barrand : if (std::abs(normal[1]) > std::abs(normal[iz])) iz = 1;
  //G.Barrand : if (std::abs(normal[2]) > std::abs(normal[iz])) iz = 2;
  if (::fabs(normal[1]) > ::fabs(normal[iz])) iz = 1; //G.Barrand
  if (::fabs(normal[2]) > ::fabs(normal[iz])) iz = 2; //G.Barrand
  if (normal[iz] > 0) {
    ix = (iz+1)%3; iy = (ix+1)%3;
  }else{
    iy = (iz+1)%3; ix = (iy+1)%3;
  }

  //   F I L L   L I S T S   O F   C O N T O U R S

  external_contours.clear();
  internal_contours.clear();
  double z;
  int    i1, i2, ifirst, iedge, icontour = faces[iface].iold;
  while (icontour > 0) {
    iedge  = icontour;
    ifirst = edges[iedge].i1;
    z      = 0.0;
    for(;;) {
      if (iedge > 0) {
        i1 = edges[iedge].i1;
        i2 = edges[iedge].i2;
        ExtNode& node_1 = nodes[i1];
        ExtNode& node_2 = nodes[i2];
        z += node_1.v[ix]*node_2.v[iy]-node_2.v[ix]*node_1.v[iy];
        if (ifirst != i2) {
          iedge = edges[iedge].inext;
          continue;
        }else{
          if (z > del*del) {
            external_contours.push_back(icontour);
          }else if (z < -del*del) {
            internal_contours.push_back(icontour);
          }else{
            processor_error = 1;
#ifdef BP_DEBUG
            G4cerr
              << "BooleanProcessor::triangulateFace : too small contour"
              << G4endl;
#endif
          }
          icontour = edges[iedge].inext;
          edges[iedge].inext = 0;
          break;
        }
      }else{
        processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::triangulateFace : broken contour"
          << G4endl;
#endif
        icontour = 0;
        break;
      }
    }
  }

  //   G E T   R I D   O F   I N T E R N A L   C O N T O U R S

  int kint, kext;
  for (kint=0; kint < (int)internal_contours.size(); kint++) {
    for (kext=0; kext < (int)external_contours.size(); kext++) {
      mergeContours(ix, iy, kext, kint);
      if (internal_contours[kint] == 0) break;
    }
    if (kext == (int)external_contours.size()) {
      processor_error = 1;
#ifdef BP_DEBUG
      G4cerr
        << "BooleanProcessor::triangulateFace : "
        << "could not merge internal contour " << kint
        << G4endl;
#endif
    }
  }

  //   T R I A N G U L A T E   C O N T O U R S

  int nface = faces.size();
  for (kext=0; kext < (int)external_contours.size(); kext++) {
    triangulateContour(ix, iy, external_contours[kext]);
#ifdef BP_DEBUG
    if(processor_error) { //G.Barrand
      G4cerr
        << "BooleanProcessor::triangulateFace : "
        << "triangulateContour failed."
        << G4endl;
      break; //G.Barrand : ok ?
    }
#endif
  }
  faces[iface].inew = UNSUITABLE_FACE;

  //   M O D I F Y   R E F E R E N C E S

  for (int ifa=nface; ifa<(int)faces.size(); ifa++) {
    iedge = faces[ifa].iold;
    while (iedge > 0) {
      if (edges[iedge].iface1 != ifa) {
        processor_error = 1;
#ifdef BP_DEBUG
        G4cerr
          << "BooleanProcessor::triangulateFace : wrong reference to itself, "
          << "iface=" << ifa << ", iface1=" << edges[iedge].iface1
          << G4endl;
#endif
      }else if (edges[iedge].iface2 > 0) {
        modifyReference(edges[iedge].iface2,
                        edges[iedge].i1, edges[iedge].i2, ifa);
      }else if (edges[iedge].iface2 < 0) {
        edges[iedge].iface2 = edges[-edges[iedge].iface2].iface1;
      }
      iedge = edges[iedge].inext;
    }
  }
}

HepPolyhedron BooleanProcessor::createPolyhedron()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::createPolyhedron()        Date:    14.03.00 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Create HepPolyhedron.                                     *
 *                                                                     *
 ***********************************************************************/
{
  int i, iedge, nnode = 0, nface = 0;

  //   R E N U M E R A T E   N O D E S   A N D   F A C E S

  for (i=1; i<(int)nodes.size(); i++) nodes[i].s = 0;

  for (i=1; i<(int)faces.size(); i++) {
    if (faces[i].inew == ORIGINAL_FACE) {
      faces[i].inew = ++nface;
      iedge = faces[i].iold;
      while (iedge > 0) {
        nodes[edges[iedge].i1].s = 1;
        iedge = edges[iedge].inext;
      }
    }else{
      faces[i].inew = 0;
    }
  }

  for (i=1; i<(int)nodes.size(); i++) {
    if (nodes[i].s == 1) nodes[i].s = ++nnode;
  }

  //   A L L O C A T E   M E M O R Y

  ExtPolyhedron polyhedron;
  if (nface == 0) return polyhedron;
  polyhedron.AllocateMemory(nnode, nface);

  //   S E T   N O D E S

  for (i=1; i<(int)nodes.size(); i++) {
    if (nodes[i].s != 0)  polyhedron.pV[nodes[i].s] = nodes[i].v;
  }

  //   S E T   F A C E S

  int k, v[4], f[4];
  for (i=1; i<(int)faces.size(); i++) {
    if (faces[i].inew == 0) continue;
    v[3] = f[3] = k = 0;
    iedge = faces[i].iold;
    while (iedge > 0) {
      if (k > 3) {
        std::cerr << "BooleanProcessor::createPolyhedron : too many edges" << std::endl;
        break;
      }
      v[k]  = nodes[edges[iedge].i1].s;
      if (edges[iedge].ivis < 0) v[k] = -v[k];
      f[k]  = faces[edges[iedge].iface2].inew;
      iedge = edges[iedge].inext;
      k++;
    }
    if (k < 3) {
      std::cerr << "BooleanProcessor::createPolyhedron : "
        << "face has only " << k << " edges"
        << std::endl;
    }
    polyhedron.pF[faces[i].inew] =
      G4Facet(v[0],f[0], v[1],f[1], v[2],f[2], v[3],f[3]);
  }
  return polyhedron;
}

int BooleanProcessor::ishift = 0; //G.Barrand
int BooleanProcessor::get_shift() { return ishift;} //G.Barrand
void BooleanProcessor::set_shift(int a_shift) { ishift = a_shift;} //G.Barrand
#define NUM_SHIFT 8
int BooleanProcessor::get_num_shift() { return NUM_SHIFT;} //G.Barrand

HepPolyhedron BooleanProcessor::execute(int op,
                                        const HepPolyhedron & a,
                                        const HepPolyhedron & b,
                                        int& err) //G.Barrand
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::execute                   Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Execute boolean operation.                                *
 *                                                                     *
 ***********************************************************************/
{
  //static int ishift = 0; //G.Barrand
  //static double shift[8][3] = {
  static double shift[NUM_SHIFT][3] = { //G.Barrand
    {  31,  23,  17},
    { -31, -23, -17},
    { -23,  17,  31},
    {  23, -17, -31},
    { -17, -31,  23},
    {  17,  31, -23},
    {  31, -23,  17},
    { -31,  23, -17}
  };

/*
  G4cerr << "BooleanProcessor::execute : ++++++++++++++++++++++"
            << a.getName().getString()
            << b.getName().getString()
            << G4endl;
*/

  //   I N I T I A T E   P R O C E S S O R

  processor_error = 0;
  operation = op;
  nodes.clear(); nodes.push_back(CRAZY_POINT);
  edges.clear(); edges.push_back(ExtEdge());
  faces.clear(); faces.push_back(ExtFace(edges,0)); //G.Barrand : ok ?

  //   T A K E   P O L Y H E D R A

  ifaces1 = faces.size(); takePolyhedron(a,0,0,0);
  ifaces2 = faces.size(); takePolyhedron(b,0,0,0);

  if (processor_error) {             // corrupted polyhedron
    std::cerr
      << "BooleanProcessor: corrupted input polyhedron"
      << std::endl;
    err = processor_error; //G.Barrand
    return HepPolyhedron();
  }
  if (ifaces1 == ifaces2) {          // a is empty
    err = processor_error; //G.Barrand
    switch (operation) {
    case OP_UNION:
      return b;
    case OP_INTERSECTION:
    	std::cerr
        << "BooleanProcessor: intersection with empty polyhedron"
        << std::endl;
      return HepPolyhedron();
    case OP_SUBTRACTION:
    	std::cerr
        << "BooleanProcessor: subtraction from empty polyhedron"
        << std::endl;
      return HepPolyhedron();
    }
  }
  if (ifaces2 == (int)faces.size()) {     // b is empty
    err = processor_error; //G.Barrand
    switch (operation) {
    case OP_UNION:
      return a;
    case OP_INTERSECTION:
    	std::cerr
        << "BooleanProcessor: intersection with empty polyhedron"
        << std::endl;
      return HepPolyhedron();
    case OP_SUBTRACTION:
      return a;
    }
  }

  //   S E T   I N I T I A L   M I N - M A X   A N D   T O L E R A N C E

  del = findMinMax();

  //   W O R K A R O U N D   T O   A V O I D   I E   A N D   E E

/*
#define PROCESSOR_ERROR(a_what) \
  G4cerr << "BooleanProcessor: boolean operation problem (" << a_what \
            << "). Try again with other shifts."\
            << G4endl;
*/
#define PROCESSOR_ERROR(a_what)

  unsigned int try_count = 1;
  while(true) { //G.Barrand

  double ddxx = del*shift[ishift][0];
  double ddyy = del*shift[ishift][1];
  double ddzz = del*shift[ishift][2];
  ishift++; if (ishift == get_num_shift()) ishift = 0;

  processor_error = 0; //G.Barrand
  operation = op;
  nodes.clear(); nodes.push_back(CRAZY_POINT);
  edges.clear(); edges.push_back(ExtEdge());
  faces.clear(); faces.push_back(ExtFace(edges,0)); //G.Barrand : ok ?

  ifaces1 = faces.size(); takePolyhedron(a,0,0,0);
  ifaces2 = faces.size(); takePolyhedron(b,ddxx,ddyy,ddzz);

  if (processor_error) { PROCESSOR_ERROR(1) } //G.Barrand

  del = findMinMax();

  //   P R E S E L E C T   O U T S I D E   F A C E S

  iout1 = iout2 = 0;
  selectOutsideFaces(ifaces1, iout1);
  selectOutsideFaces(ifaces2, iout2);

  if (processor_error) { PROCESSOR_ERROR(2) } //G.Barrand

  //   P R E S E L E C T   N O   I N T E R S E C T I O N   F A C E S

  int ifa1, ifa2;
  iunk1 = iunk2 = 0;
  if (iout1 != 0 || iout2 != 0) {
    for(;;) {
      ifa1 = iunk1;
      ifa2 = iunk2;
      selectOutsideFaces(ifaces1, iunk1);
      selectOutsideFaces(ifaces2, iunk2);
      if (iunk1 == ifa1 && iunk2 == ifa2) break;
      findMinMax();
    }
  }

  if (processor_error) { PROCESSOR_ERROR(3) } //G.Barrand

  //   F I N D   N E W   E D G E S

  if (ifaces1 != 0 && ifaces2 != 0 ) {
    ifa1 = ifaces1;
    while (ifa1 > 0) {
      ifa2 = ifaces2;
      while (ifa2 > 0) {
        testFaceVsFace(ifa1, ifa2);
        ifa2 = faces[ifa2].inext;
      }
      ifa1 = faces[ifa1].inext;
    }
  }
  if (processor_error) { PROCESSOR_ERROR(4) } //G.Barrand

  //   C O N S T R U C T   N E W   F A C E S

  assembleNewFaces((operation == OP_INTERSECTION) ? 1 : 0, ifaces1);
  if (processor_error) { PROCESSOR_ERROR(5) } //G.Barrand
  assembleNewFaces((operation == OP_UNION) ? 0 : 1, ifaces2);
  if (processor_error) { PROCESSOR_ERROR(6) } //G.Barrand

  //   A S S E M B L E   S U I T A B L E   F A C E S

  initiateLists();
  assemblePolyhedra();
  if (unknown_faces.front() != 0) {
    processor_error = 1;
#ifdef BP_DEBUG
    G4cerr
      << "BooleanProcessor::execute : unknown faces !!!"
      << G4endl;
#endif
  }
  if (processor_error) { PROCESSOR_ERROR(7) } //G.Barrand

  //   T R I A N G U L A T E   A C C E P T E D   F A C E S

  ifa1 = result_faces.front();
  while (ifa1 > 0) {
    ifa2 = ifa1;
    ifa1 = faces[ifa2].inext;
    if (faces[ifa2].inew == NEW_FACE) triangulateFace(ifa2);
    if (processor_error) {
      PROCESSOR_ERROR(8) //G.Barrand
      break; //G.Barrand
    }
  }

  if(!processor_error) {
#ifdef BP_DEBUG
    if(try_count!=1) {
      G4cerr
         << "BooleanProcessor::execute : had converged."
         << G4endl;
    }
#endif
    break;
  }

  if((int)try_count>get_num_shift()) {
#ifdef BP_DEBUG
    /*** Commented out because HepPolyhedron does not have getName...?!
    G4cerr << "BooleanProcessor: "
              << " all shifts tried. Boolean operation (" << op << ") failure."
              << " a name \"" << a.getName().getString() << "\""
              << " b name \"" << b.getName().getString() << "\""
              << G4endl;
    ***/
#endif
    err = processor_error;
    return a;
  }

#ifdef BP_DEBUG
  G4cerr
     << "BooleanProcessor::execute : try another tilt..."
     << G4endl;
#endif

  try_count++;

  } //G.Barrand : end while shift.
#undef PROCESSOR_ERROR //G.Barrand

  //   C R E A T E   P O L Y H E D R O N

  err = processor_error;
  return createPolyhedron();
}


//#include <cfortran.h>
//#include <higz.h>
//#include "zbuf.h"
//void BooleanProcessor::draw()
/***********************************************************************
 *                                                                     *
 * Name: BooleanProcessor::draw                      Date:    10.12.99 *
 * Author: E.Chernyaev                               Revised:          *
 *                                                                     *
 * Function: Draw                                                      *
 *                                                                     *
 ***********************************************************************/
/*
{
  int II;
  int   icol, i1, i2, iedge, iface, ilist[4];
  float p1[3], p2[3];

  ilist[0] = ifaces1;
  ilist[1] = ifaces2;
  ilist[2] = iout1;
  ilist[3] = iout2;

  for (int i=0; i<4; i++) {

    if (i == 0) G4cout << "========= Ifaces_1" << G4endl;
    if (i == 1) G4cout << "========= Ifaces_2" << G4endl;
    if (i == 2) G4cout << "========= Iout_1" << G4endl;
    if (i == 3) G4cout << "========= Iout_2" << G4endl;

    icol = i+1;
    iface = ilist[i];
    while (iface > 0) {

      G4cout << "iface = " << iface << G4endl;
      G4cout << "--- iold" << G4endl;

      iedge = faces[iface].iold;
      icol = 2;

      while (iedge > 0) {

        G4cout << "  iegde = " << iedge
             << " i1,i2 =" << edges[iedge].i1 << "," << edges[iedge].i2
             << " iface1,iface2 = "
             << edges[iedge].iface1 << "," << edges[iedge].iface2
             << G4endl;

        i1 = edges[iedge].i1;
        p1[0] = nodes[i1].v.x();
        p1[1] = nodes[i1].v.y();
        p1[2] = nodes[i1].v.z();
        IHWTON(p1,p1);
        i2 = edges[iedge].i2;
        p2[0] = nodes[i2].v.x();
        p2[1] = nodes[i2].v.y();
        p2[2] = nodes[i2].v.z();
        IHWTON(p2,p2);
//        icol =  (edges[iedge].ivis > 0) ? 1 : 2;
        IHZLIN(icol,p1[0],p1[1],p1[2], p2[0],p2[1],p2[2]);
        iedge = edges[iedge].inext;
      }

      G4cout << "--- inew" << G4endl;

      iedge = faces[iface].inew;
      icol = 3;

      while (iedge > 0) {

        G4cout << "  iegde = " << iedge
             << " i1,i2 =" << edges[iedge].i1 << "," << edges[iedge].i2
             << " iface1,iface2 = "
             << edges[iedge].iface1 << "," << edges[iedge].iface2
             << G4endl;

        i1 = edges[iedge].i1;
        p1[0] = nodes[i1].v.x();
        p1[1] = nodes[i1].v.y();
        p1[2] = nodes[i1].v.z();
        IHWTON(p1,p1);
        i2 = edges[iedge].i2;
        p2[0] = nodes[i2].v.x();
        p2[1] = nodes[i2].v.y();
        p2[2] = nodes[i2].v.z();
        IHWTON(p2,p2);
//        icol =  (edges[iedge].ivis > 0) ? 1 : 2;
        IHZLIN(icol,p1[0],p1[1],p1[2], p2[0],p2[1],p2[2]);
        iedge = edges[iedge].inext;
      }
      iface = faces[iface].inext;

      IHZTOX(0,100,100);
      ixupdwi(0);
      cin >> II;
      ixclrwi();
      IHZCLE(0);
    }
  }
}
*/

/*
//--------------------------------------------------------------------
void
BooleanProcessor::draw_edge(int icol, int iedge) {
  int   i1, i2;
  float p1[3], p2[3];

  i1 = edges[iedge].i1;
  p1[0] = nodes[i1].v.x();
  p1[1] = nodes[i1].v.y();
  p1[2] = nodes[i1].v.z();
  IHWTON(p1,p1);
  i2 = edges[iedge].i2;
  p2[0] = nodes[i2].v.x();
  p2[1] = nodes[i2].v.y();
  p2[2] = nodes[i2].v.z();
  IHWTON(p2,p2);
  IHZLIN(icol,p1[0],p1[1],p1[2], p2[0],p2[1],p2[2]);
}

//--------------------------------------------------------------------
void
BooleanProcessor::draw_contour(int i1col, int i2col, int ihead) {
  int iedge, icol;
  iedge = ihead;
  while (iedge > 0) {
    icol = (edges[iedge].ivis > 0) ? i1col : i2col;
    draw_edge(icol, iedge);
    iedge = edges[iedge].inext;
  }

  IHZTOX(0,100,100);
  ixupdwi(0);

  int i;
  std::cin >> i;
}

//--------------------------------------------------------------------
void
BooleanProcessor::print_face(int iface) {
  G4cout.precision(3);
  G4cout << "\n====== Face N " << iface << G4endl;
  G4cout << "iedges[4] = "
       << faces[iface].iedges[0] << ", "
       << faces[iface].iedges[1] << ", "
       << faces[iface].iedges[2] << ", "
       << faces[iface].iedges[3] << G4endl;
  G4cout << "rmin[3] = "
       << faces[iface].rmin[0] << ", "
       << faces[iface].rmin[1] << ", "
       << faces[iface].rmin[2] << G4endl;
  G4cout << "rmax[3] = "
       << faces[iface].rmax[0] << ", "
       << faces[iface].rmax[1] << ", "
       << faces[iface].rmax[2] << G4endl;
  G4cout << "iprev,inext = "
       << faces[iface].iprev << ", "
       << faces[iface].inext << G4endl;
  G4cout << "iold = " << faces[iface].iold << G4endl;
  for(int i = faces[iface].iold; i != 0;) {
    print_edge(i);
    i = edges[abs(i)].inext;
  }

  G4cout << "inew = ";
  switch (faces[iface].inew) {
  case UNKNOWN_FACE:
    G4cout << "UNKNOWN_FACE" << G4endl;
    break;
  case ORIGINAL_FACE:
    G4cout << "ORIGINAL_FACE" << G4endl;
    break;
  case NEW_FACE:
    G4cout << "NEW_FACE" << G4endl;
    break;
  case UNSUITABLE_FACE:
    G4cout << "UNSUITABLE_FACE" << G4endl;
    break;
  case DEFECTIVE_FACE:
    G4cout << "DEFECTIVE_FACE" << G4endl;
    break;
  default:
    G4cout << faces[iface].inew << G4endl;
    for(int k = faces[iface].inew; k != 0;) {
      print_edge(k);
      k = edges[abs(k)].inext;
    }
  }
}

//--------------------------------------------------------------------
void
BooleanProcessor::print_edge(int iedge) {
  G4cout << "==== Edge N " << iedge << G4endl;
  int i = std::abs(iedge);
  int i1 = edges[i].i1;
  int i2 = edges[i].i2;
  G4cout << "node[" << i1 << "] = "
       << nodes[i1].v.x() << ", "
       << nodes[i1].v.y() << ", "
       << nodes[i1].v.z() << G4endl;

  G4cout << "node[" << i2 << "] = "
       << nodes[i2].v.x() << ", "
       << nodes[i2].v.y() << ", "
       << nodes[i2].v.z() << G4endl;

  G4cout << "iface1,iface2,ivis,inext = "
       << edges[i].iface1 << ", "
       << edges[i].iface2 << ", "
       << edges[i].ivis   << ", "
       << edges[i].inext  << G4endl;
}
*/

void BooleanProcessor::dump() {//G.Barrand
  unsigned int number = nodes.size();
  std::cerr << "nodes : " << number << std::endl;
  for(unsigned int index=0;index<number;index++) {
    const ExtNode& node = nodes[index];
    std::cerr << " " << index
           << " x = " << node.v[0]
           << " y = " << node.v[1]
           << " z = " << node.v[2]
           << std::endl;
  }
}
