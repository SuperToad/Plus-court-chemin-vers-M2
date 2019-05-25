#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ChenHanImproved/BaseModel.h"
#include "ChenHanImproved/RichModel.h"
#include "ChenHanImproved/ExactMethodForDGP.h"
#include "ChenHanImproved/PreviousCH.h"
#include "ChenHanImproved/ImprovedCHWithFilteringRule.h"
#include "ChenHanImproved/XinWangImprovedCH.h"
#include <QDebug>
#include <QElapsedTimer>


void MainWindow::showSelections(MyMesh* _mesh){
    // on réinitialise les couleurs de tout le maillage
    resetAllColorsAndThickness(_mesh);

    if(face1 < 0 || face1 >= _mesh->n_faces()){
        qDebug() << "f1 out of bound !";
        qDebug() << "max =" << _mesh->n_faces();
    }else{
        FaceHandle fh = _mesh->face_handle(face1);
        _mesh->set_color(fh, MyMesh::Color(0, 255, 0));
        MyMesh::FaceVertexIter fh_v = _mesh->fv_iter(fh);
        pointStart = MyMesh::Point(0);
        unsigned iter = 0;
        for(; fh_v.is_valid(); ++fh_v)
        {
            VertexHandle vertexHandle = *fh_v;

            switch (iter)
            {
                case 0:pointStart += _mesh->point(vertexHandle)*ui->slider_dx1->value()/100;break;
                case 1:pointStart += _mesh->point(vertexHandle)*ui->slider_dy1->value()/100;break;
                case 2:pointStart += _mesh->point(vertexHandle)*ui->slider_dz1->value()/100;break;
                default:pointStart += _mesh->point(vertexHandle);
            }
            iter++;
        }
//        pointStart/=3;

        ui->spinbox_x1->setValue(pointStart[0]);
        ui->spinbox_y1->setValue(pointStart[1]);
        ui->spinbox_z1->setValue(pointStart[2]);
    }

    if( face2 < 0 || face2 >= _mesh->n_faces()){
        qDebug() << "f2 out of bound !";
        qDebug() << "max =" << _mesh->n_faces();
    }
    else{
        FaceHandle fh = _mesh->face_handle(face2);
        _mesh->set_color(fh, MyMesh::Color(0, 255, 0));
        MyMesh::FaceVertexIter fh_v = _mesh->fv_iter(fh);
        pointEnd = MyMesh::Point(0);
        unsigned iter = 0;
        for(; fh_v.is_valid(); ++fh_v)
        {
            VertexHandle vertexHandle = *fh_v;
            switch (iter)
            {
                case 0:pointEnd += _mesh->point(vertexHandle)*ui->slider_dx2->value()/100;break;
                case 1:pointEnd += _mesh->point(vertexHandle)*ui->slider_dy2->value()/100;break;
                case 2:pointEnd += _mesh->point(vertexHandle)*ui->slider_dz2->value()/100;break;
                default:pointEnd += _mesh->point(vertexHandle);
            }
            iter++;
        }
//        pointEnd/=3;

        ui->spinbox_x2->setValue(pointEnd[0]);
        ui->spinbox_y2->setValue(pointEnd[1]);
        ui->spinbox_z2->setValue(pointEnd[2]);
    }

    // on affiche le nouveau maillage
    displayMesh(_mesh);
}

// Dijkstra
int MainWindow::trouveMin(MyMesh* _mesh)
{
    int vMin = -1;
    float distMin = -1;
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

float MainWindow::calculDistances( MyMesh::Point p1, MyMesh::Point p2){
    float dx = p1[0] - p2[0];
    float dy = p1[1] - p2[1];
    float dz = p1[2] - p2[2];
    return sqrt( dx*dx + dy*dy + dz*dz );
}

void MainWindow::majDistances(MyMesh* _mesh, int v1)
{
    VertexHandle vh1 = _mesh->vertex_handle(v1);
    float distV1 = _mesh->data(vh1).dist;
    for (MyMesh::VertexVertexIter VertexNeighbour=_mesh->vv_iter(vh1); VertexNeighbour.is_valid(); ++VertexNeighbour)
    {
        VertexHandle vh2 = *VertexNeighbour;
        float newDist=distV1 + calculDistances( _mesh->point(vh1), _mesh->point(vh2));
        if (_mesh->data(vh2).dist == -1 || _mesh->data(vh2).dist > newDist)
        {
            _mesh->data(vh2).dist = newDist;
            _mesh->data(vh2).pred = v1;
        }
    }
    if(_mesh->data(vh1).isEnd)
    {
        float newDist=distV1 + calculDistances( _mesh->point(vh1), pointEnd);
        if (finalDist == -1 || finalDist > newDist)
        {
            finalDist = newDist;
            finalPred = v1;
        }
    }
}

int MainWindow::Dijkstra (MyMesh* _mesh, int faceStart, int faceEnd)
{
    //initialisation
    for (MyMesh::VertexIter v_it=_mesh->vertices_sbegin(); v_it!=_mesh->vertices_end(); ++v_it)
    {
        _mesh->data(*v_it).dist = -1;               //endless distance
        _mesh->data(*v_it).checked = false;         //unchek
        _mesh->data(*v_it).isEnd = false;           //unchek
        _mesh->data(*v_it).pred = (*v_it).idx();    //self predecessor
    }

    MyMesh::FaceVertexIter fv_it = _mesh->fv_iter(_mesh->face_handle(faceStart));
    for(; fv_it.is_valid(); ++fv_it)
    {
        _mesh->data(*fv_it).dist=calculDistances(_mesh->point(*fv_it),pointStart);
    }

    fv_it = _mesh->fv_iter(_mesh->face_handle(faceEnd));
    for(; fv_it.is_valid(); ++fv_it)
    {
        _mesh->data(*fv_it).isEnd=true;
    }

    int currentVertex=trouveMin(_mesh);
    finalDist=-1;
    finalPred=-1;
    //fin initialisation


    while (finalDist == -1 || finalDist > _mesh->data(_mesh->vertex_handle(currentVertex)).dist )
    {
        majDistances(_mesh, currentVertex);
        _mesh->data(_mesh->vertex_handle(currentVertex)).checked = true; //chek currentVertex
        currentVertex = trouveMin(_mesh);

        if(currentVertex == -1){
            if(finalDist==-1){
                qDebug() << "pas de chemin trouvé" ;
                return -1;
            }
            return 0;
        }
    }
    return 0;
}

void MainWindow::showPath(MyMesh* _mesh)
{
    if(face1 < 0 || face1 >= _mesh->n_faces()){
        qDebug() << "v1 out of bound !";
        qDebug() << "max =" << _mesh->n_faces();
        return;
    }if( face2 < 0 || face2 >= _mesh->n_faces()){
        qDebug() << "v2 out of bound !";
        qDebug() << "max =" << _mesh->n_faces();
        return;
    }

    QElapsedTimer timer;
    timer.start();
    if(Dijkstra(_mesh, face1, face2) == 0){
        qDebug() << "Distance avec Dijkstra : " << finalDist;
        qDebug() << "Temps de calcul avec Dijkstra : " << timer.elapsed() << "ms";
        timer.invalidate();
        for (MyMesh::VertexIter v_it=_mesh->vertices_sbegin(); v_it!=_mesh->vertices_end(); ++v_it)
        {
            if(_mesh->data(*v_it).checked == true){
                _mesh->set_color(v_it, MyMesh::Color(255, 0, 0));
                _mesh->data(v_it).thickness = 2;
            }
        }
        // on réinitialise l'affichage
        resetAllColorsAndThickness(_mesh);

        //affichage des face à défaut des points
        FaceHandle vh1 = _mesh->face_handle(face1);
        FaceHandle vh2 = _mesh->face_handle(face2);
        _mesh->set_color(vh1, MyMesh::Color(0, 255, 0));
        _mesh->set_color(vh2, MyMesh::Color(0, 255, 0));

        for (MyMesh::VertexIter v_it=_mesh->vertices_sbegin(); v_it!=_mesh->vertices_end(); ++v_it)
        {
            if(_mesh->data(*v_it).checked == true){
                _mesh->set_color(v_it, MyMesh::Color(255, 0, 0));
                _mesh->data(v_it).thickness = 4;
            }
        }

        VertexHandle currentVertex = _mesh->vertex_handle(finalPred);
        _mesh->set_color(currentVertex, MyMesh::Color(0, 255, 255));
        _mesh->data(currentVertex).thickness = 5;
        VertexHandle previousVertex = currentVertex;
        currentVertex = _mesh->vertex_handle(_mesh->data(currentVertex).pred);
        while(currentVertex != previousVertex){
            _mesh->set_color(currentVertex, MyMesh::Color(0, 255, 255));
            _mesh->data(currentVertex).thickness = 5;

            for (MyMesh::VertexEdgeIter edgeTmp1= _mesh->ve_iter(currentVertex);edgeTmp1.is_valid();++edgeTmp1) {
                for (MyMesh::VertexEdgeIter edgeTmp2= _mesh->ve_iter(previousVertex);edgeTmp2.is_valid();++edgeTmp2) {
                    if((*edgeTmp1).idx() == (*edgeTmp2).idx()){
                        _mesh->set_color(edgeTmp1, MyMesh::Color(0, 200, 200));
                        _mesh->data(edgeTmp1).thickness = 3;
                    }
                }
            }
            previousVertex = currentVertex;
            currentVertex = _mesh->vertex_handle(_mesh->data(currentVertex).pred);
        }

        /*
        // point de départ et point d'arrivée en vert et en gros
        _mesh->set_color(vh1, MyMesh::Color(0, 255, 0));
        _mesh->data(vh1).thickness = 9;
        _mesh->set_color(vh2, MyMesh::Color(0, 255, 0));
        _mesh->data(vh2).thickness = 9;*/

    }
    // on affiche le nouveau maillage
    displayMesh(_mesh);

}
void MainWindow::showPathGeo(MyMesh* _mesh)
{
    if(face1 < 0 || face1 >= _mesh->n_faces()){
        qDebug() << "v1 out of bound !";
        qDebug() << "max =" << _mesh->n_faces();
        return;
    }if( face2 < 0 || face2 >= _mesh->n_faces()){
        qDebug() << "v2 out of bound !";
        qDebug() << "max =" << _mesh->n_faces();
        return;
    }
    resetAllColorsAndThickness(_mesh);

    QElapsedTimer timer;
    timer.start();
    if(Dijkstra(_mesh, face1, face2) == 0){
        qDebug() << "Temps de calcul partie Dijkstra : " << timer.elapsed() << "ms";
        qDebug() << "Distance avec Dijkstra : " << finalDist;

        //affichage des face à défaut des points
        FaceHandle vh1 = _mesh->face_handle(face1);
        FaceHandle vh2 = _mesh->face_handle(face2);
        _mesh->set_color(vh1, MyMesh::Color(0, 255, 0));
        _mesh->set_color(vh2, MyMesh::Color(0, 255, 0));

        VertexHandle currentVertex = _mesh->vertex_handle(finalPred);
        VertexHandle previousVertex = currentVertex;
        currentVertex = _mesh->vertex_handle(_mesh->data(currentVertex).pred);
        while(currentVertex != previousVertex){
            previousVertex = currentVertex;
            currentVertex = _mesh->vertex_handle(_mesh->data(currentVertex).pred);
        }
        int vertex1 = _mesh->vertex_handle(finalPred).idx();
        int vertex2 = currentVertex.idx();

        // on affiche le nouveau maillage
        displayMesh(_mesh);

        // Debut utilisation CHI
        CExactMethodForDGP * algorithm = new CXinWangImprovedCH(*model, vertex1);
        timer.restart();
        algorithm->Execute();
        vector<CPoint3D> resultpoints;
        if (!(vertex2 < 0 || vertex2 >=  model->GetNumOfVerts()))
        {
            algorithm->BackTrace(vertex2, resultpoints);
            qDebug() << "Temps de calcul partie geodesique : " << timer.elapsed() << "ms";
            qDebug() << "Distance avec geodesique : " << calculDistances(resultpoints);
            /*for (int i = resultpoints.size() -1; i >=0; --i)
            {
                qDebug() << "(" << resultpoints[i].x << ", " << resultpoints[i].y << ", " << resultpoints[i].z << ")";
            }*/
        }
        timer.invalidate();
        // Fin utilisation CHI
        // Affichage chemin obtenu avec CHI, a faire apres displayMesh
        displayPath(resultpoints);
    }

}

float MainWindow::calculDistances(vector<CPoint3D> resultpoints)
{
    float result = 0.0f;
    float dx = pointStart[0] - resultpoints[0].x;
    float dy = pointStart[1] - resultpoints[0].y;
    float dz = pointStart[2] - resultpoints[0].z;
    result += sqrt( dx*dx + dy*dy + dz*dz );
    for (int i = 0; i < resultpoints.size() - 1; i++)
    {
        dx = resultpoints[i].x - resultpoints[i + 1].x;
        dy = resultpoints[i].y - resultpoints[i + 1].y;
        dz = resultpoints[i].z - resultpoints[i + 1].z;
        result += sqrt( dx*dx + dy*dy + dz*dz );
    }
    dx = resultpoints[resultpoints.size() - 1].x - pointEnd[0];
    dy = resultpoints[resultpoints.size() - 1].y - pointEnd[1];
    dz = resultpoints[resultpoints.size() - 1].z - pointEnd[2];
    result += sqrt( dx*dx + dy*dy + dz*dz );

    return result;
}


void MainWindow::on_pushButton_afficherChemin_clicked()
{
    // on récupère les sommets de départ et d'arrivée
    int indexV1 = ui->spinBox_v1_chemin->value();
    int indexV2 = ui->spinBox_v2_chemin->value();

    showPath(&mesh);
}

void MainWindow::on_pushButton_afficherCheminGeo_clicked()
{
    // on récupère les sommets de départ et d'arrivée
    int indexV1 = ui->spinBox_v1_chemin->value();
    int indexV2 = ui->spinBox_v2_chemin->value();

    showPathGeo(&mesh);
}

void MainWindow::enableSliders(bool enable)
{
    ui->slider_dx1->setEnabled(enable);
    ui->slider_dy1->setEnabled(enable);
    ui->slider_dz1->setEnabled(enable);
    ui->slider_dx2->setEnabled(enable);
    ui->slider_dy2->setEnabled(enable);
    ui->slider_dz2->setEnabled(enable);

    ui->slider_dx1->setValue(33);
    ui->slider_dy1->setValue(33);
    ui->slider_dz1->setValue(33);
    ui->slider_dx2->setValue(33);
    ui->slider_dy2->setValue(33);
    ui->slider_dz2->setValue(33);

    ui->spinBox_v1_chemin->setReadOnly(!enable);
    ui->spinBox_v2_chemin->setReadOnly(!enable);
}

void MainWindow::on_pushButton_chargement_clicked()
{
    enableSliders(false);
    // fenêtre de sélection des fichiers
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Mesh"), "", tr("Mesh Files (*.obj)"));

    // chargement du fichier .obj dans la variable globale "mesh"
    OpenMesh::IO::read_mesh(mesh, fileName.toUtf8().constData());

    // initialisation des couleurs et épaisseurs (sommets et arêtes) du mesh
    resetAllColorsAndThickness(&mesh);


    // Debut ouverture du modele avec CHI
    model = new CRichModel(fileName.toUtf8().data());
    qDebug() <<"----------------------model info begin----------------\n";
    qDebug() << "File name:\t" << model->GetFileName().data() << endl;
    try
    {
        model->LoadModel();
    }
    catch(const char* msg)
    {
        qDebug() << "ERRORS happen!\n" << msg;
    }
    model->Preprocess();
    if (!model->HasBeenLoad() || !model->HasBeenProcessed())
    {
        qDebug() << "The model fails to be handled.";
    }
    // Fin ouverture du modele avec CHI

    // on affiche le maillage
    displayMesh(&mesh);
    if (mesh.n_vertices() > 0)
    {
        enableSliders(true);
        ui->spinBox_v1_chemin->setMaximum(mesh.n_faces());
        ui->spinBox_v2_chemin->setMaximum(mesh.n_faces());
    }
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

    ui->displayWidget->resetPath();
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

    // Affichage points sur la face
    if (_mesh->n_vertices() > 0)
    {
        displayStartFacePoint();
        displayEndFacePoint();
    }
}

// charge un chemin geodesique dans l'environnement OpenGL
void MainWindow::displayPath(vector<CPoint3D> resultpoints)
{
    GLuint* triIndiceArray = new GLuint[(resultpoints.size()+2) * 3];
    GLfloat* triCols = new GLfloat[(resultpoints.size()+2) * 3 * 3];
    GLfloat* triVerts = new GLfloat[(resultpoints.size()+2) * 3 * 3];

    QList<QPair<float, int> > vertsSizes;
    // Point de debut
    int i = 0;
    triCols[3*i+0] = 0; triCols[3*i+1] = 255; triCols[3*i+2] = 255;
    triVerts[3*i+0] = pointStart[0]; triVerts[3*i+1] = pointStart[1]; triVerts[3*i+2] = pointStart[2];
    triIndiceArray[i] = i;
    vertsSizes.append(qMakePair(15, i));
    i++;
    // Points de la geodesique
    // On utilise "i-1" quand on est en decalage avec le tableau resultpoints
    for (; i - 1 < resultpoints.size() ; ++i)
    {
        triCols[3*i+0] = 0; triCols[3*i+1] = 255; triCols[3*i+2] = 255;
        triVerts[3*i+0] = resultpoints[i-1].x; triVerts[3*i+1] = resultpoints[i-1].y; triVerts[3*i+2] = resultpoints[i-1].z;
        triIndiceArray[i] = i;
        vertsSizes.append(qMakePair(10, i));
    }
    // Point de fin
    triCols[3*i+0] = 0; triCols[3*i+1] = 255; triCols[3*i+2] = 255;
    triVerts[3*i+0] = pointEnd[0]; triVerts[3*i+1] = pointEnd[1]; triVerts[3*i+2] = pointEnd[2];
    triIndiceArray[i] = i;
    vertsSizes.append(qMakePair(15, i));
    i++;

    ui->displayWidget->loadPoints(triVerts, triCols, i * 3, triIndiceArray, i, vertsSizes);

    delete[] triIndiceArray;
    delete[] triCols;
    delete[] triVerts;

    GLuint* linesIndiceArray = new GLuint[(resultpoints.size()+2) * 2];
    GLfloat* linesCols = new GLfloat[(resultpoints.size()+2) * 2 * 3];
    GLfloat* linesVerts = new GLfloat[(resultpoints.size()+2) * 2 * 3];

    QList<QPair<float, int> > edgeSizes;
    i = 0;
    int j = 0;
    // arete de la face de debut
    linesVerts[3*i+0] = pointStart[0]; linesVerts[3*i+1] = pointStart[1]; linesVerts[3*i+2] = pointStart[2];
    linesCols[3*i+0] = 0; linesCols[3*i+1] = 200; linesCols[3*i+2] = 200;
    linesIndiceArray[i] = i;
    edgeSizes.append(qMakePair(3, i));
    i++;

    linesVerts[3*i+0] = resultpoints[j].x; linesVerts[3*i+1] = resultpoints[j].y; linesVerts[3*i+2] = resultpoints[j].z;
    linesCols[3*i+0] = 0; linesCols[3*i+1] = 200; linesCols[3*i+2] = 200;
    linesIndiceArray[i] = i;
    edgeSizes.append(qMakePair(3, i));
    i++;
    // aretes intermediaires
    while ( i < resultpoints.size()*2 - 1 )
    {
        linesVerts[3*i+0] = resultpoints[j].x; linesVerts[3*i+1] = resultpoints[j].y; linesVerts[3*i+2] = resultpoints[j].z;
        linesCols[3*i+0] = 0; linesCols[3*i+1] = 200; linesCols[3*i+2] = 200;
        linesIndiceArray[i] = i;
        edgeSizes.append(qMakePair(3, i));
        i++;
        j++;

        linesVerts[3*i+0] = resultpoints[j].x; linesVerts[3*i+1] = resultpoints[j].y; linesVerts[3*i+2] = resultpoints[j].z;
        linesCols[3*i+0] = 0; linesCols[3*i+1] = 200; linesCols[3*i+2] = 200;
        linesIndiceArray[i] = i;
        edgeSizes.append(qMakePair(3, i));
        i++;
    }
    // arete de la face de fin
    linesVerts[3*i+0] = resultpoints[j].x; linesVerts[3*i+1] = resultpoints[j].y; linesVerts[3*i+2] = resultpoints[j].z;
    linesCols[3*i+0] = 0; linesCols[3*i+1] = 200; linesCols[3*i+2] = 200;
    linesIndiceArray[i] = i;
    edgeSizes.append(qMakePair(3, i));
    i++;

    linesVerts[3*i+0] = pointEnd[0]; linesVerts[3*i+1] = pointEnd[1]; linesVerts[3*i+2] = pointEnd[2];
    linesCols[3*i+0] = 0; linesCols[3*i+1] = 200; linesCols[3*i+2] = 200;
    linesIndiceArray[i] = i;
    edgeSizes.append(qMakePair(3, i));
    i++;

    ui->displayWidget->loadPath(linesVerts, linesCols, i * 3, linesIndiceArray, i, edgeSizes);

    delete[] linesIndiceArray;
    delete[] linesCols;
    delete[] linesVerts;

}

// charge un chemin geodesique dans l'environnement OpenGL
void MainWindow::displayStartFacePoint()
{
    GLuint* triIndiceArray = new GLuint[2];
    GLfloat* triCols = new GLfloat[2* 3];
    GLfloat* triVerts = new GLfloat[2 * 3];

    QList<QPair<float, int> > vertsSizes;
    // Point de debut
    triCols[0] = 255;               triCols[1] = 0;                 triCols[2] = 0;
    triVerts[0] = pointStart[0];    triVerts[1] = pointStart[1];    triVerts[2] = pointStart[2];
    triIndiceArray[0] = 0;
    vertsSizes.append(qMakePair(10, 0));
    // Point de fin
    triCols[3] = 255;               triCols[4] = 0;                 triCols[5] = 0;
    triVerts[3] = pointEnd[0];      triVerts[4] = pointEnd[1];      triVerts[5] = pointEnd[2];
    triIndiceArray[1] = 1;
    vertsSizes.append(qMakePair(10, 1));

    ui->displayWidget->loadStartFacePoint(triVerts, triCols, 6, triIndiceArray, 2, vertsSizes);

    delete[] triIndiceArray;
    delete[] triCols;
    delete[] triVerts;

}
void MainWindow::displayEndFacePoint()
{
    GLuint* triIndiceArray = new GLuint[2];
    GLfloat* triCols = new GLfloat[2* 3];
    GLfloat* triVerts = new GLfloat[2 * 3];

    QList<QPair<float, int> > vertsSizes;
    // Point de fin
    triCols[0] = 255;             triCols[1] = 0;               triCols[2] = 0;
    triVerts[0] = pointEnd[0];    triVerts[1] = pointEnd[1];    triVerts[2] = pointEnd[2];
    triIndiceArray[0] = 0;
    vertsSizes.append(qMakePair(10, 0));
    // Point de fin
    triCols[3] = 255;               triCols[4] = 0;                 triCols[5] = 0;
    triVerts[3] = pointEnd[0];      triVerts[4] = pointEnd[1];      triVerts[5] = pointEnd[2];
    triIndiceArray[1] = 1;
    vertsSizes.append(qMakePair(10, 1));

    ui->displayWidget->loadEndFacePoint(triVerts, triCols, 6, triIndiceArray, 2, vertsSizes);

    delete[] triIndiceArray;
    delete[] triCols;
    delete[] triVerts;

}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    vertexSelection = -1;
    edgeSelection = -1;
    faceSelection = -1;

    modevoisinage = false;

    ui->setupUi(this);
    enableSliders(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_spinBox_v1_chemin_valueChanged(int arg1)
{
    face1=arg1;
    showSelections(&mesh);
}

void MainWindow::on_spinBox_v2_chemin_valueChanged(int arg1)
{
    face2=arg1;
    showSelections(&mesh);
}

void MainWindow::on_slider_dx1_valueChanged(int arg1)
{
    if (!changingSlider)
    {
        changingSlider = true;
        int x1 = arg1;
        int y1 = ui->slider_dy1->value();
        int z1 = ui->slider_dz1->value();

        int partA = (y1+z1==0?1:y1+z1);
        int partB = 100-x1;

        y1 = y1*partB/partA;
        z1 = z1*partB/partA;

        if (x1+y1+z1 < 100)
        {
            if (z1 > y1) z1++;
            else y1++;
        }


        ui->slider_dx1->setValue(x1);
        ui->slider_dy1->setValue(y1);
        ui->slider_dz1->setValue(z1);

        changingSlider = false;
        showSelections(&mesh);
    }
}
void MainWindow::on_slider_dy1_valueChanged(int arg1)
{
    if (!changingSlider)
    {
        changingSlider = true;
        int y1 = arg1;
        int x1 = ui->slider_dx1->value();
        int z1 = ui->slider_dz1->value();

        int partA = (x1+z1==0?1:x1+z1);
        int partB = 100-y1;

        x1 = x1*partB/partA;
        z1 = z1*partB/partA;

        if (x1+y1+z1 < 100)
        {
            if (x1 > z1) x1++;
            else z1++;
        }

        ui->slider_dx1->setValue(x1);
        ui->slider_dy1->setValue(y1);
        ui->slider_dz1->setValue(z1);

        changingSlider = false;
        showSelections(&mesh);
    }
}
void MainWindow::on_slider_dz1_valueChanged(int arg1)
{
    if (!changingSlider)
    {
        changingSlider = true;
        int z1 = arg1;
        int y1 = ui->slider_dy1->value();
        int x1 = ui->slider_dx1->value();

        int partA = (y1+x1==0?1:y1+x1);
        int partB = 100-z1;

        y1 = y1*partB/partA;
        x1 = x1*partB/partA;

        if (x1+y1+z1 < 100)
        {
            if (x1 > y1) x1++;
            else y1++;
        }

        ui->slider_dx1->setValue(x1);
        ui->slider_dy1->setValue(y1);
        ui->slider_dz1->setValue(z1);

        changingSlider = false;
        showSelections(&mesh);
    }
}
void MainWindow::on_slider_dx2_valueChanged(int arg1)
{
    if (!changingSlider)
    {
        changingSlider = true;
        int x2 = arg1;
        int y2 = ui->slider_dy2->value();
        int z2 = ui->slider_dz2->value();

        int partA = (y2+z2==0?1:y2+z2);
        int partB = 100-x2;

        y2 = y2*partB/partA;
        z2 = z2*partB/partA;

        if (x2+y2+z2 < 100)
        {
            if (z2 > y2) z2++;
            else y2++;
        }

        ui->slider_dx2->setValue(x2);
        ui->slider_dy2->setValue(y2);
        ui->slider_dz2->setValue(z2);

        changingSlider = false;
        showSelections(&mesh);
    }
}
void MainWindow::on_slider_dy2_valueChanged(int arg1)
{
    if (!changingSlider)
    {
        changingSlider = true;
        int y2 = arg1;
        int x2 = ui->slider_dx2->value();
        int z2 = ui->slider_dz2->value();

        int partA = (x2+z2==0?1:x2+z2);
        int partB = 100-y2;

        x2 = x2*partB/partA;
        z2 = z2*partB/partA;

        if (x2+y2+z2 < 100)
        {
            if (x2 > z2) x2++;
            else z2++;
        }

        ui->slider_dx2->setValue(x2);
        ui->slider_dy2->setValue(y2);
        ui->slider_dz2->setValue(z2);

        changingSlider = false;
        showSelections(&mesh);
    }
}
void MainWindow::on_slider_dz2_valueChanged(int arg1)
{
    if (!changingSlider)
    {
        changingSlider = true;
        int z2 = arg1;
        int y2 = ui->slider_dy2->value();
        int x2 = ui->slider_dx2->value();

        int partA = (y2+x2==0?1:y2+x2);
        int partB = 100-z2;

        y2 = y2*partB/partA;
        x2 = x2*partB/partA;

        if (x2+y2+z2 < 100)
        {
            if (x2 > y2) x2++;
            else y2++;
        }

        ui->slider_dx2->setValue(x2);
        ui->slider_dy2->setValue(y2);
        ui->slider_dz2->setValue(z2);

        changingSlider = false;
        showSelections(&mesh);
    }
}
