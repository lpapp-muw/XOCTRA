# XOCTRA
Repository for the Xenograft OCT Radiomics Analysis project (MeduniWien)

## Intended Use
We explicitly state that our solution does not describe any product and it is not intended to be used in any real-life, especially not in clinical settings. This repository contains description and examples of a research tool for experimenting with OCT and radiomics. Therefore, the contents of this repository shall not be used either partly or fully for rendering clinical decisions or to be included in any product. We waive any liabilities for any damage caused by the executables.

## License
This repository and its contents are licensed under the Creative Commons Attribution-Noncommercial-NoDerivatives license 4.0 (CC BY-NC-ND 4.0).

In case you wish to use our solution in a commercial environment please contact the Technology Transfer Office (TTO) of MUV: https://www.meduniwien.ac.at/technologietransfer

Contact the corresponding author (Fabian Placzek, PhD, e-mail: fabian.placzek@meduniwien.ac.at) in case you are interested in research collaborations regarding the utilization of DEBI-NNs.

## Access
See below the Use Cases with accompanying data and source code descriptions we utilized for this study.

### Use Case 1: Tiling OCT images and their corresponding masks
In this use case, the OCT slices and their respective binary masks are processed. First, the image pairs are loaded up, followed by morphological closing of the mask image. Afterwards, small non-overlapping tile images are generated (default: 80x80 pixels) from which radiomic features can be extracted.

Under ```ToTile``` is an example OCT case with its corresponding binary mask which can be used as input for the C++ code (see below) to generate tile images.

### Use Case 2: Radiomics redundancy clustering and ranking
In this use case, a pre-generated correlation matrix (```CM.csv```) and a correlation with Progression (```CL.csv```) are analysed. ```CM``` contains all radiomic features Spearman ranked to one another. ```CL``` contains the Spearman ranks of each radiomic feature with the label Progression. These matrices were generated from ```radiomics.csv```. All these files are under folder ```Radiomics```. A Spearman Rank-based clustering is performed in ```CM``` to build up redundant cluster groups, from which the feature with the highest Spearman Rank to Progression (```CL```) is selected per-cluster.

### How to compile the C++ code
Our source code having Use Cases 1 and 2 are in folder ```Source```. This is a Visual Studio project, requiring ```Qt 5.12.0``` or higher to compile. To install Qt, refer to the official installation guide: doc.qt.io

Under ```Source``` the python files ```spearman_ranking.py``` and ```correlation_matrix.py``` can be found that were used to extract radiomic features from the tiles (```pyRadiomics v3.0.1```) and to perform Spearman ranking, respectively.


## Citation
Our paper is currently under review. This repository is intended to be used only by our reviewers at this time.
