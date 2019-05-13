#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileDialog>
#include <QMainWindow>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include "ChenHanImproved/RichModel.h"
#include <math.h>

namespace Ui {
class MainWindow;
}

using namespace OpenMesh;
using namespace OpenMesh::Attributes;

struct MyTraits : public OpenMesh::DefaultTraits
{
    // use vertex normals and vertex colors
    VertexAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color );
    // store the previous halfedge
    HalfedgeAttributes( OpenMesh::Attributes::PrevHalfedge );
    // use face normals face colors
    FaceAttributes( OpenMesh::Attributes::Normal | OpenMesh::Attributes::Color );
    EdgeAttributes( OpenMesh::Attributes::Color );
    // vertex thickness
    VertexTraits{float thickness;float dist;int pred; bool checked; bool isEnd;};
    // edge thickness
    EdgeTraits{float thickness;};
};
typedef OpenMesh::TriMesh_ArrayKernelT<MyTraits> MyMesh;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // les 4 fonctions à compléter
    void showSelections(MyMesh* _mesh);
    //void showSelectionsNeighborhood(MyMesh* _mesh);
    void showPath(MyMesh* _mesh);
    //void showBorder(MyMesh* _mesh);

    // Dijkstra
    int trouveMin(MyMesh* _mesh);
    float calculDistances(MyMesh::Point p1, MyMesh::Point p2);
    void majDistances(MyMesh* _mesh, int v);
    int Dijkstra (MyMesh* _mesh, int faceStart, int faceEnd);

    void displayMesh(MyMesh *_mesh);
    void displayPath(vector<CPoint3D> resultpoints);
    void resetAllColorsAndThickness(MyMesh* _mesh);

private slots:

    void on_pushButton_chargement_clicked();
//    void on_pushButton_vertexMoins_clicked();
//    void on_pushButton_vertexPlus_clicked();
//    void on_pushButton_edgeMoins_clicked();
//    void on_pushButton_edgePlus_clicked();
//    void on_pushButton_faceMoins_clicked();
//    void on_pushButton_facePlus_clicked();
    void on_pushButton_afficherChemin_clicked();
//    void on_pushButton_voisinage_clicked();
//    void on_pushButton_bordure_clicked();

    void on_spinBox_v1_chemin_valueChanged(int arg1);

    void on_spinBox_v2_chemin_valueChanged(int arg1);

private:

    bool modevoisinage;

    int vertexSelection;
    int edgeSelection;
    int faceSelection;

    MyMesh mesh;
    int face1=0,face2=0;    //face sur les quelles sont les points sélectionnés
    float finalDist;
    int finalPred;
    MyMesh::Point pointStart, pointEnd;

    CRichModel *model;


    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
