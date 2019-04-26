#include "mainwindow.h"
#include "ui_mainwindow.h"

/* **** début de la partie à compléter **** */

void MainWindow::showSelections(MyMesh* _mesh)
{
    // on réinitialise les couleurs de tout le maillage
    resetAllColorsAndThickness(_mesh);

    if(faceSelection > -1) {
        FaceHandle faceHandle = _mesh->face_handle(faceSelection);
        _mesh->set_color(faceHandle, MyMesh::Color(50,50,255));

        MyMesh::FaceEdgeIter fh_it = mesh.fe_iter(faceHandle);
        for(; fh_it.is_valid(); ++fh_it) {
            EdgeHandle edgeHandle = *fh_it;
            _mesh->set_color(edgeHandle, MyMesh::Color(0, 0, 255));
            _mesh->data(edgeHandle).thickness = 3;
        }

        MyMesh::FaceVertexIter fh_v = mesh.fv_iter(faceHandle);
        for(; fh_v.is_valid(); ++fh_v) {
            VertexHandle vertexHandle = *fh_v;
            _mesh->set_color(vertexHandle, MyMesh::Color(0, 0, 255));
            _mesh->data(vertexHandle).thickness = 7;
        }
    }

    if(edgeSelection > -1) {
        EdgeHandle edgeHandle = _mesh->edge_handle(edgeSelection);
        _mesh->set_color(edgeHandle, MyMesh::Color(0, 255, 0));
        _mesh->data(edgeHandle).thickness = 3;

        HalfedgeHandle heh0 = _mesh->halfedge_handle(edgeHandle, 0); // la première demi-arête
        HalfedgeHandle heh1 = _mesh->halfedge_handle(edgeHandle, 1); // la seconde demi-arête

        VertexHandle v = _mesh->to_vertex_handle(heh0);
        _mesh->set_color(v, MyMesh::Color(0,255,0));
        _mesh->data(v).thickness = 7;
        v = _mesh->to_vertex_handle(heh1);
        _mesh->set_color(v, MyMesh::Color(0,255,0));
        _mesh->data(v).thickness = 7;
    }

    if(vertexSelection > -1) {
        VertexHandle vertexHandle = _mesh->vertex_handle(vertexSelection);
        _mesh->set_color(vertexHandle, MyMesh::Color(255, 0, 0));
        _mesh->data(vertexHandle).thickness = 7;
    }

    // on affiche le nouveau maillage
    displayMesh(_mesh);
}


void MainWindow::showSelectionsNeighborhood(MyMesh* _mesh)
{
    // on réinitialise les couleurs de tout le maillage
    resetAllColorsAndThickness(_mesh);

    if(faceSelection > -1) {
        FaceHandle faceHandle = _mesh->face_handle(faceSelection);


        MyMesh::FaceEdgeIter fh_it = mesh.fe_iter(faceHandle);
        for(; fh_it.is_valid(); ++fh_it) {
            EdgeHandle edgeHandle = *fh_it;
            _mesh->set_color(edgeHandle, MyMesh::Color(0, 0, 255));
            _mesh->data(edgeHandle).thickness = 3;

            HalfedgeHandle heh0 = _mesh->halfedge_handle(edgeHandle, 0); // la première demi-arête
            HalfedgeHandle heh1 = _mesh->halfedge_handle(edgeHandle, 1); // la seconde demi-arête

            FaceHandle fh = _mesh->face_handle(heh0);
            _mesh->set_color(fh, MyMesh::Color(100,100,255));

            fh = _mesh->face_handle(heh1);
            _mesh->set_color(fh, MyMesh::Color(100,100,255));
        }

        _mesh->set_color(faceHandle, MyMesh::Color(50,50,255));

        MyMesh::FaceVertexIter fh_v = mesh.fv_iter(faceHandle);
        for(; fh_v.is_valid(); ++fh_v) {
            VertexHandle vertexHandle = *fh_v;
            _mesh->set_color(vertexHandle, MyMesh::Color(0, 0, 255));
            _mesh->data(vertexHandle).thickness = 7;
        }
    }

    if(edgeSelection > -1) {
        EdgeHandle edgeHandle = _mesh->edge_handle(edgeSelection);
        _mesh->set_color(edgeHandle, MyMesh::Color(0, 255, 0));
        _mesh->data(edgeHandle).thickness = 3;

        HalfedgeHandle heh0 = _mesh->halfedge_handle(edgeHandle, 0); // la première demi-arête
        HalfedgeHandle heh1 = _mesh->halfedge_handle(edgeHandle, 1); // la seconde demi-arête

        FaceHandle fh = _mesh->face_handle(heh0);
        _mesh->set_color(fh, MyMesh::Color(100,255,100));

        fh = _mesh->face_handle(heh1);
        _mesh->set_color(fh, MyMesh::Color(100,255,100));

        VertexHandle v = _mesh->to_vertex_handle(heh0);
        _mesh->set_color(v, MyMesh::Color(0,255,0));
        _mesh->data(v).thickness = 7;
        v = _mesh->to_vertex_handle(heh1);
        _mesh->set_color(v, MyMesh::Color(0,255,0));
        _mesh->data(v).thickness = 7;
    }

    if(vertexSelection > -1) {
        VertexHandle vertexHandle = _mesh->vertex_handle(vertexSelection);
        _mesh->set_color(vertexHandle, MyMesh::Color(255, 0, 0));
        _mesh->data(vertexHandle).thickness = 7;

        MyMesh::VertexEdgeIter v_it = mesh.ve_iter(vertexHandle);
        for(; v_it.is_valid(); ++v_it) {
            EdgeHandle edgeHandle = *v_it;
            _mesh->set_color(edgeHandle, MyMesh::Color(255, 0, 0));
            _mesh->data(edgeHandle).thickness = 3;
        }
    }
    displayMesh(_mesh);
}



void MainWindow::showBorder(MyMesh* _mesh)
{
    // on réinitialise l'affichage
    resetAllColorsAndThickness(_mesh);

    for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++)
    {
        EdgeHandle eh = *curEdge;
        if(_mesh->is_boundary(eh)){
            _mesh->set_color(eh, MyMesh::Color(0, 255, 255));
            _mesh->data(eh).thickness = 3;
        }

    }
    displayMesh(_mesh);
}

// Dijkstra
int MainWindow::trouveMin(MyMesh* _mesh)
{
    int vMin = -1;
    double distMin = -1;
    for (MyMesh::VertexIter v_it=_mesh->vertices_sbegin(); v_it!=_mesh->vertices_end(); ++v_it)
    {
        if (_mesh->data(*v_it).dist != -1 && _mesh->data(*v_it).checked == false && (_mesh->data(*v_it).dist < distMin || distMin == -1))
        {
            distMin = _mesh->data(*v_it).dist;
            vMin = (*v_it).idx();
        }
    }
    return vMin;
}

void MainWindow::majDistances(MyMesh* _mesh, int v1, int v2)
{
    VertexHandle vh1 = _mesh->vertex_handle(v1);
    VertexHandle vh2 = _mesh->vertex_handle(v2);
    double dx = _mesh->point(vh1)[0] - _mesh->point(vh2)[0];
    double dy = _mesh->point(vh1)[1] - _mesh->point(vh2)[1];
    double dz = _mesh->point(vh1)[2] - _mesh->point(vh2)[2];
    double DistV1V2= sqrt( dx*dx + dy*dy + dz*dz );
    double newDist=_mesh->data(vh1).dist + DistV1V2;
    if (_mesh->data(vh2).dist == -1 || _mesh->data(vh2).dist > newDist)
    {
        _mesh->data(vh2).dist = newDist;
        _mesh->data(vh2).pred = v1;
    }
}

int MainWindow::Dijkstra (MyMesh* _mesh, int VertexStart, int VertexEnd)
{
    for (MyMesh::VertexIter v_it=_mesh->vertices_sbegin(); v_it!=_mesh->vertices_end(); ++v_it)
    {
        _mesh->data(*v_it).dist = -1;               //endless distance
        _mesh->data(*v_it).checked = false;         //unchek
        _mesh->data(*v_it).pred = (*v_it).idx();    //self predecessor
    }
    VertexHandle vh1 = _mesh->vertex_handle(VertexStart);
    _mesh->data(vh1).dist = 0;

    int currentVertex=VertexStart;
    while (currentVertex != VertexEnd)
    {
        VertexHandle vh1 = _mesh->vertex_handle(currentVertex);
        for (MyMesh::VertexVertexIter VertexNeighbour=_mesh->vv_iter(vh1); VertexNeighbour.is_valid(); ++VertexNeighbour)
        {
            majDistances(_mesh, currentVertex, (*VertexNeighbour).idx());
        }
        _mesh->data(vh1).checked = true; //chek currentVertex
        currentVertex = trouveMin(_mesh);
        if(currentVertex == -1){
            qDebug() << "pas de chemin trouvé" ;
            return -1;
        }
    }
    return 0;
}

void MainWindow::showPath(MyMesh* _mesh, int v1, int v2)
{
    if(v1 < 0 || v1 >= _mesh->n_vertices()){
        qDebug() << "v1 out of bound !";
        qDebug() << "max =" << _mesh->n_vertices();
        return;
    }if( v2 < 0 || v2 >= _mesh->n_vertices()){
        qDebug() << "v2 out of bound !";
        qDebug() << "max =" << _mesh->n_vertices();
        return;
    }

    // on réinitialise l'affichage
    resetAllColorsAndThickness(_mesh);

    VertexHandle vh1 = _mesh->vertex_handle(v1);
    VertexHandle vh2 = _mesh->vertex_handle(v2);

    // point de départ et point d'arrivée en vert et en gros
    _mesh->set_color(vh1, MyMesh::Color(0, 255, 0));
    _mesh->data(vh1).thickness = 9;
    _mesh->set_color(vh2, MyMesh::Color(0, 255, 0));
    _mesh->data(vh2).thickness = 9;

    if(Dijkstra(_mesh, v1, v2) == 0){
        VertexHandle currentVertex = vh2;
        currentVertex = _mesh->vertex_handle(_mesh->data(currentVertex).pred);
        while(currentVertex != vh1){
            _mesh->set_color(currentVertex, MyMesh::Color(0, 255, 255));
            _mesh->data(currentVertex).thickness = 5;
            currentVertex = _mesh->vertex_handle(_mesh->data(currentVertex).pred);
        }
    }




    // on affiche le nouveau maillage
    displayMesh(_mesh);
}

/* **** fin de la partie à compléter **** */


/* **** début de la partie boutons et IHM **** */
/*
void MainWindow::on_pushButton_bordure_clicked()
{
    showBorder(&mesh);
}*/

//void MainWindow::on_pushButton_voisinage_clicked()
//{
//    // changement de mode entre avec et sans voisinage
//    if(modevoisinage)
//    {
//        ui->pushButton_voisinage->setText("Repasser en mode normal");
//        modevoisinage = false;
//    }
//    else
//    {
//        ui->pushButton_voisinage->setText("Passer en mode voisinage");
//        modevoisinage = true;
//    }

//    // on montre la nouvelle selection
//    if(!modevoisinage)
//        showSelections(&mesh);
//    else
//        showSelectionsNeighborhood(&mesh);
//}


//void MainWindow::on_pushButton_vertexMoins_clicked()
//{
//    // mise à jour de l'interface
//    vertexSelection = vertexSelection - 1;
//    ui->labelVertex->setText(QString::number(vertexSelection));

//    // on montre la nouvelle selection
//    if(!modevoisinage)
//        showSelections(&mesh);
//    else
//        showSelectionsNeighborhood(&mesh);
//}

//void MainWindow::on_pushButton_vertexPlus_clicked()
//{
//    // mise à jour de l'interface
//    vertexSelection = vertexSelection + 1;
//    ui->labelVertex->setText(QString::number(vertexSelection));

//    // on montre la nouvelle selection
//    if(!modevoisinage)
//        showSelections(&mesh);
//    else
//        showSelectionsNeighborhood(&mesh);
//}

//void MainWindow::on_pushButton_edgeMoins_clicked()
//{
//    // mise à jour de l'interface
//    edgeSelection = edgeSelection - 1;
//    ui->labelEdge->setText(QString::number(edgeSelection));

//    // on montre la nouvelle selection
//    if(!modevoisinage)
//        showSelections(&mesh);
//    else
//        showSelectionsNeighborhood(&mesh);
//}

//void MainWindow::on_pushButton_edgePlus_clicked()
//{
//    // mise à jour de l'interface
//    edgeSelection = edgeSelection + 1;
//    ui->labelEdge->setText(QString::number(edgeSelection));

//    // on montre la nouvelle selection
//    if(!modevoisinage)
//        showSelections(&mesh);
//    else
//        showSelectionsNeighborhood(&mesh);
//}

//void MainWindow::on_pushButton_faceMoins_clicked()
//{
//    // mise à jour de l'interface
//    faceSelection = faceSelection - 1;
//    ui->labelFace->setText(QString::number(faceSelection));

//    // on montre la nouvelle selection
//    if(!modevoisinage)
//        showSelections(&mesh);
//    else
//        showSelectionsNeighborhood(&mesh);
//}

//void MainWindow::on_pushButton_facePlus_clicked()
//{
//    // mise à jour de l'interface
//    faceSelection = faceSelection + 1;
//    ui->labelFace->setText(QString::number(faceSelection));

//    // on montre la nouvelle selection
//    if(!modevoisinage)
//        showSelections(&mesh);
//    else
//        showSelectionsNeighborhood(&mesh);
//}

void MainWindow::on_pushButton_afficherChemin_clicked()
{
    // on récupère les sommets de départ et d'arrivée
    int indexV1 = ui->spinBox_v1_chemin->value();
    int indexV2 = ui->spinBox_v2_chemin->value();

    showPath(&mesh, indexV1, indexV2);
}


void MainWindow::on_pushButton_chargement_clicked()
{
    // fenêtre de sélection des fichiers
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Mesh"), "", tr("Mesh Files (*.obj)"));

    // chargement du fichier .obj dans la variable globale "mesh"
    OpenMesh::IO::read_mesh(mesh, fileName.toUtf8().constData());

    // initialisation des couleurs et épaisseurs (sommets et arêtes) du mesh
    resetAllColorsAndThickness(&mesh);

    // on affiche le maillage
    displayMesh(&mesh);
}

/* **** fin de la partie boutons et IHM **** */



/* **** fonctions supplémentaires **** */

// permet d'initialiser les couleurs et les épaisseurs des élements du maillage
void MainWindow::resetAllColorsAndThickness(MyMesh* _mesh)
{
    for (MyMesh::VertexIter curVert = _mesh->vertices_begin(); curVert != _mesh->vertices_end(); curVert++)
    {
        _mesh->data(*curVert).thickness = 1;
        _mesh->set_color(*curVert, MyMesh::Color(0, 0, 0));
    }

    for (MyMesh::FaceIter curFace = _mesh->faces_begin(); curFace != _mesh->faces_end(); curFace++)
    {
        _mesh->set_color(*curFace, MyMesh::Color(150, 150, 150));
    }

    for (MyMesh::EdgeIter curEdge = _mesh->edges_begin(); curEdge != _mesh->edges_end(); curEdge++)
    {
        _mesh->data(*curEdge).thickness = 1;
        _mesh->set_color(*curEdge, MyMesh::Color(0, 0, 0));
    }
}

// charge un objet MyMesh dans l'environnement OpenGL
void MainWindow::displayMesh(MyMesh* _mesh)
{
    GLuint* triIndiceArray = new GLuint[_mesh->n_faces() * 3];
    GLfloat* triCols = new GLfloat[_mesh->n_faces() * 3 * 3];
    GLfloat* triVerts = new GLfloat[_mesh->n_faces() * 3 * 3];

    MyMesh::ConstFaceIter fIt(_mesh->faces_begin()), fEnd(_mesh->faces_end());
    MyMesh::ConstFaceVertexIter fvIt;
    int i = 0;
    for (; fIt!=fEnd; ++fIt)
    {
        fvIt = _mesh->cfv_iter(*fIt);
        triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
        triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
        triIndiceArray[i] = i;

        i++; ++fvIt;
        triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
        triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
        triIndiceArray[i] = i;

        i++; ++fvIt;
        triCols[3*i+0] = _mesh->color(*fIt)[0]; triCols[3*i+1] = _mesh->color(*fIt)[1]; triCols[3*i+2] = _mesh->color(*fIt)[2];
        triVerts[3*i+0] = _mesh->point(*fvIt)[0]; triVerts[3*i+1] = _mesh->point(*fvIt)[1]; triVerts[3*i+2] = _mesh->point(*fvIt)[2];
        triIndiceArray[i] = i;

        i++;
    }

    ui->displayWidget->loadMesh(triVerts, triCols, _mesh->n_faces() * 3 * 3, triIndiceArray, _mesh->n_faces() * 3);

    delete[] triIndiceArray;
    delete[] triCols;
    delete[] triVerts;

    GLuint* linesIndiceArray = new GLuint[_mesh->n_edges() * 2];
    GLfloat* linesCols = new GLfloat[_mesh->n_edges() * 2 * 3];
    GLfloat* linesVerts = new GLfloat[_mesh->n_edges() * 2 * 3];

    i = 0;
    QHash<float, QList<int> > edgesIDbyThickness;
    for (MyMesh::EdgeIter eit = _mesh->edges_begin(); eit != _mesh->edges_end(); ++eit)
    {
        float t = _mesh->data(*eit).thickness;
        if(t > 0)
        {
            if(!edgesIDbyThickness.contains(t))
                edgesIDbyThickness[t] = QList<int>();
            edgesIDbyThickness[t].append((*eit).idx());
        }
    }
    QHashIterator<float, QList<int> > it(edgesIDbyThickness);
    QList<QPair<float, int> > edgeSizes;
    while (it.hasNext())
    {
        it.next();

        for(int e = 0; e < it.value().size(); e++)
        {
            int eidx = it.value().at(e);

            MyMesh::VertexHandle vh1 = _mesh->to_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh1)[0];
            linesVerts[3*i+1] = _mesh->point(vh1)[1];
            linesVerts[3*i+2] = _mesh->point(vh1)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;

            MyMesh::VertexHandle vh2 = _mesh->from_vertex_handle(_mesh->halfedge_handle(_mesh->edge_handle(eidx), 0));
            linesVerts[3*i+0] = _mesh->point(vh2)[0];
            linesVerts[3*i+1] = _mesh->point(vh2)[1];
            linesVerts[3*i+2] = _mesh->point(vh2)[2];
            linesCols[3*i+0] = _mesh->color(_mesh->edge_handle(eidx))[0];
            linesCols[3*i+1] = _mesh->color(_mesh->edge_handle(eidx))[1];
            linesCols[3*i+2] = _mesh->color(_mesh->edge_handle(eidx))[2];
            linesIndiceArray[i] = i;
            i++;
        }
        edgeSizes.append(qMakePair(it.key(), it.value().size()));
    }

    ui->displayWidget->loadLines(linesVerts, linesCols, i * 3, linesIndiceArray, i, edgeSizes);

    delete[] linesIndiceArray;
    delete[] linesCols;
    delete[] linesVerts;

    GLuint* pointsIndiceArray = new GLuint[_mesh->n_vertices()];
    GLfloat* pointsCols = new GLfloat[_mesh->n_vertices() * 3];
    GLfloat* pointsVerts = new GLfloat[_mesh->n_vertices() * 3];

    i = 0;
    QHash<float, QList<int> > vertsIDbyThickness;
    for (MyMesh::VertexIter vit = _mesh->vertices_begin(); vit != _mesh->vertices_end(); ++vit)
    {
        float t = _mesh->data(*vit).thickness;
        if(t > 0)
        {
            if(!vertsIDbyThickness.contains(t))
                vertsIDbyThickness[t] = QList<int>();
            vertsIDbyThickness[t].append((*vit).idx());
        }
    }
    QHashIterator<float, QList<int> > vitt(vertsIDbyThickness);
    QList<QPair<float, int> > vertsSizes;

    while (vitt.hasNext())
    {
        vitt.next();

        for(int v = 0; v < vitt.value().size(); v++)
        {
            int vidx = vitt.value().at(v);

            pointsVerts[3*i+0] = _mesh->point(_mesh->vertex_handle(vidx))[0];
            pointsVerts[3*i+1] = _mesh->point(_mesh->vertex_handle(vidx))[1];
            pointsVerts[3*i+2] = _mesh->point(_mesh->vertex_handle(vidx))[2];
            pointsCols[3*i+0] = _mesh->color(_mesh->vertex_handle(vidx))[0];
            pointsCols[3*i+1] = _mesh->color(_mesh->vertex_handle(vidx))[1];
            pointsCols[3*i+2] = _mesh->color(_mesh->vertex_handle(vidx))[2];
            pointsIndiceArray[i] = i;
            i++;
        }
        vertsSizes.append(qMakePair(vitt.key(), vitt.value().size()));
    }

    ui->displayWidget->loadPoints(pointsVerts, pointsCols, i * 3, pointsIndiceArray, i, vertsSizes);

    delete[] pointsIndiceArray;
    delete[] pointsCols;
    delete[] pointsVerts;
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    vertexSelection = -1;
    edgeSelection = -1;
    faceSelection = -1;

    modevoisinage = false;

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

