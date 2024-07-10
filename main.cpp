/**
 * @file main.cpp
 * @brief Disparity map estimation using bilaterally weighted patches
 * @author Laura F. Julia <fernandl@imagine.enpc.fr>
 *         Pascal Monasse <monasse@imagine.enpc.fr>
 *
 * Copyright (c) 2014-2015, Laura F. Julia, Pascal Monasse
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Pulic License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "cmdLine.h"
#include "io_png.h"
#include <iostream>
#include "node.h"

/// Load color image
Image loadImage(const char* name) {
    size_t width, height;
    float* pix = io_png_read_f32_rgb(name, &width, &height);
    if(! pix) {
        std::cerr << "Unable to read file " << name << " as PNG" << std::endl;
        std::exit(1);
    }
    const int w=static_cast<int>(width), h=static_cast<int>(height);
    Image im(w, h, 3);
    const float *r=pix, *g=r+w*h, *b=g+w*h;
    for(int y=0; y<h; y++)
        for(int x=0; x<w; x++) {
            im(x,y,0) = *r++;
            im(x,y,1) = *g++;
            im(x,y,2) = *b++;
        }
    std::free(pix);
    return im;
}



/// Main program
int main(int argc, char *argv[]) {


    if (argc!=2 ){ //a demander pourquoi argc=2 sur mac et 1 sur windows
        std::cout<<argc<<std::endl;
        std::cout<<"Une seule image demandée"<<std::endl;
        return 0; //here we only want one picture --> test
    }

    int patchSize=31;
    int lmin=3;
    int lmax=20;
    int thresholdConfusion =-patchSize*patchSize*600;//à diminuer
    int thresholdSimilarity=patchSize*patchSize*600;//à diminuer




    //imageInput is located in argv[1]+'/baseball.png'
    std::string imagePath = std::string(argv[1]) + "/baseball.png";
    Image imageInput=loadImage(imagePath.c_str());


    //The mask is located in argv[1]+'/mask_baseball.png'
    std::string maskPath = std::string(argv[1]) + "/mask_baseball.png";
    Image imageMaskTemp=loadImage(maskPath.c_str());

    Image imageMask(imageMaskTemp.gray().clone());
    //Image imageMask=imageMaskTemp.simplifyMaskToOnePixel(162,95,210,161);

    if(! save_image(std::string(std::string(argv[1]) + "/imageMask.png").c_str(), imageMask)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }

    Image imageExtendedMask=imageMask.gray().extendMask(patchSize);

    if(! save_image(std::string(std::string(argv[1]) + "/imageExtendedMask.png").c_str(), imageExtendedMask)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }




    std::vector<Node> v=nodesOverMask(imageExtendedMask,patchSize,lmax);

    Image maskOverImage= visualiseMaskOverImage(imageInput,imageMask);

    if(! save_image(std::string(std::string(argv[1]) + "/maskOverImage.png").c_str(), maskOverImage)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }



    Image nodesAndVertices=visualiseNodesAndVertices(imageMask,v,patchSize);

    // Utile pour enregistrer l'image
    if(! save_image(std::string(std::string(argv[1]) + "/nodesAndVertices.png").c_str(), nodesAndVertices)){
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }









    std::vector<Node> priorities=assignInitialPriority(imageInput,imageExtendedMask,imageMask,
                                                         patchSize,lmin,lmax,thresholdConfusion,thresholdSimilarity);



    Image confusionSet=getConfusionSet(priorities,imageInput,patchSize,lmax);

    if(! save_image(std::string(std::string(argv[1]) + "/confusionSet.png").c_str(), confusionSet)){
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }

    pourcentageNoeudPruned(priorities,lmax,lmin);


    for (size_t i=0;i<v.size();i++){
        Node nodeCandidate=priorities[getNodeOfIndex(priorities,i)];
        if (nodeCandidate.size()==0) continue;
        Image candidates=visualizeCandidate(priorities,imageInput,patchSize,i);
        std::string name=std::string(std::string(argv[1]) + "/candidates/x"+std::to_string(nodeCandidate.getx())+"y"+std::to_string(nodeCandidate.gety())+"i"+std::to_string(nodeCandidate.getIndex())+".png");
        if (!save_image(name.c_str(),candidates)){
            std::cerr << "Error writing file "+name << std::endl;
            return 1;
        }
    }



    Image orderOfVisit;

    std::vector<int> commitStack =forwardPass(priorities,imageInput,imageExtendedMask,orderOfVisit,patchSize,thresholdSimilarity,thresholdConfusion,lmin,lmax);


    pourcentageNoeudPruned(priorities,lmax,lmin);


    if(! save_image(std::string(std::string(argv[1]) + "/order_visit.png").c_str(), orderOfVisit)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }
    orderOfVisit=backwardPass(priorities,commitStack,imageInput,imageExtendedMask,patchSize,thresholdSimilarity,thresholdConfusion,lmin,lmax);


    pourcentageNoeudPruned(priorities,lmax,lmin);









    Image reconstructed=imageReconstructed(priorities,patchSize,imageInput,imageMask);



    if(! save_image(std::string(std::string(argv[1]) + "/reconstructed.png").c_str(), reconstructed)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }





















    //int* centers=im1.listPatchCenters(patchSize);
    //std::cout<<(centers[1])<<std::endl;





    //int ip=im1.getPatchIndexFromCoordinates(200,200,patchSize);
    //Image ssdImage = im1.createSSDImage(patchSize,ip);


    /*
    // Utile pour enregistrer l'image
    if(! save_image(argv[2], ssdImage)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }
    */

    //ip=im1.getPatchIndexFromCoordinates(50,50,patchSize);
    //ssdImage = im1.createSSDImage(patchSize,ip);
    /*
    // Utile pour enregistrer l'image
    if(! save_image("/Users/felixfourreau/Desktop/projet_vacances/images/ssd_ppd2.png", ssdImage)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }*/

    /*
    if(width!=im2.width() || height!=im2.height()) {
        std::cerr << "The images must have the same size!" << std::endl;
        return 1;
    }
    */

    /*
    // Utile pour enregistrer les valeurs de paramêtre (à conserver)
    int dMin, dMax;
    if(! ((std::istringstream(argv[3])>>dMin).eof() &&
          (std::istringstream(argv[4])>>dMax).eof())) {
            std::cerr << "Error reading dMin or dMax" << std::endl;
            return 1;
    */



    /*
    // Utile pour enregistrer l'image
    if(! save_disparity("/Users/felixfourreau/Desktop/projet_vacances/images/ssd_ppd.png", ssdImage, 0, 255)) {
        std::cerr << "Error writing file " << std::endl;
        return 1;
    }
    */


    /*
    int sense=0; // Camera motion direction: '0'=to-right, '1'=to-left
    CmdLine cmd;
    cmd.prefixDoc = "    ";
    const std::string sec1("Adaptive weights parameters:"),
        sec2("Occlusion detection:"), sec3("Densification:");

    ParamDisparity paramD; // Parameters for adaptive weights
    cmd.section = sec1;
    cmd.add( make_option(0,paramD.gammaCol,"gcol")
             .doc("gamma for color difference") );

    cmd.add( make_option(0,paramD.gammaPos,"gpos")
             .doc("gamma for spatial distance") );
    cmd.add( make_option('R',paramD.radius).doc("radius of patch window") );
    cmd.add( make_option('A',paramD.alpha)
             .doc("value of alpha for matching cost") );
    cmd.add( make_option('t',paramD.tauCol)
             .doc("threshold of color difference in matching cost") );
    cmd.add( make_option('g',paramD.tauGrad)
             .doc("threshold of gradient difference in matching cost") );

    ParamOcclusion paramOcc; // Parameters for filling occlusions
    cmd.section = sec2;
    cmd.add( make_option('o',paramOcc.tol_disp)
             .doc("tolerance for left-right disp. diff.") ); // Detect occlusion
    cmd.section = sec3;
    cmd.add( make_option('O',sense)
             .doc("camera sense='0':right, '1':left") ); // Fill occlusion
    cmd.add( make_option('r',paramOcc.median_radius)
             .doc("radius of the weighted median filter") );
    cmd.add( make_option('c',paramOcc.sigma_color)
             .doc("value of sigma_color") );
    cmd.add( make_option('s',paramOcc.sigma_space)
             .doc("value of sigma_space") );

    try {
        cmd.process(argc, argv);
    } catch(std::string str) {
        std::cerr << "Error: " << str << std::endl<<std::endl;
        argc=1; // To display usage
    }

    if(argc!=5 && argc!=6) {
        std::cerr <<"Bilaterally weighted patches for disparity map computation"
                  << "\nUsage: " << argv[0]
                  << " [options] im1.png im2.png dmin dmax [out_prefix]\n\n"
                  << "Options (default values in parentheses)\n";
        std::cerr << sec1 << '\n' << CmdLine(cmd, sec1);
        std::cerr << cmd.prefixDoc << "Combination of weights is '" << COMB
                  << "' (recompile to change it)\n\n";
        std::cerr << sec2 << '\n' << CmdLine(cmd, sec2) << '\n';
        std::cerr << sec3 << '\n' << CmdLine(cmd, sec3);
        return 1;
    }

    if(!paramD.check() || !paramOcc.check())
        return 1;
    */

    return 0;
}
