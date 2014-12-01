# ObjIdent #

Given a large image set, this project's goal is to identify a specific object in an input image using the human-in-the-loop technique, combining some visual features and distance learning. For example, identify the species of tree, bird or flower.

## Compiling ##

The code depends on some 3rd party libraries, which are Qt, OpenCV, TBB, Eigen and FFTW. The Eigen library is already included in the source code. FFTW is a light weight library for computing the discrete Fourier transform, which can be found at "http://www.fftw.org/". The whole project is managed by CMake, so the rest libraries can be easily found if installed. To compile it, first configure the project using cmake or cmake-gui, then use the generator selected.

## Usage ##
### Data layouts ###
For the train dataset, images of the same group should be put under the same folder. For example, the rest layouts are valid,

    ---root dir---
        ---category 1 dir---
            ---image 1---
            ---image 2---
                ...
            ---image n---
        ---category 2 dir---
            ...
        ---category n dir---

    ---root dir---
        ---cat 1 dir---
            ---subcat 1 dir---
                ---image 1---
                ---image 2---
                    ...
            ---subcat 2 dir---
                ...
            ---subcat n dir---
        ---cat 2 dir---
            ...
        ---cat n dir---

### Program usage ###
Follow the number on the buttons.

When loading the dataset, select the root directory.

When first generate the visual features, it might take some time depending on the dataset size. After that the pre-computed features (train_features.xml and svm_model) will be put under the same folder of the program, to use it, move them to the root directory of the train dataset so next time the program will load them instead of computing.

Use mouse to select the object in the image, press "3) select" when done.

During each iteration, use the radio button to give each image a "response". 'S' is similar, 'D' is dissimilar and 'U' is unknown. Then press "5) Update Candidates". Repeat this step until you're satisfied with the result, then press "6) Finish"
