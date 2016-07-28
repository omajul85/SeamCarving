# Seam Carving for TuttleOFX

The goal of the project is developing and algorithm of SeamCarving for the open source library TuttleOFX which is based on the standard OpenFX.

### Seam Carving

Seam carving (or liquid rescaling) is an algorithm for content-aware image resizing. It functions by establishing a number of seams (paths of least importance) in an image and automatically removes seams to reduce image size or inserts seams to extend it. Seam carving also allows manually defining areas in which pixels may not be modified, and features the ability to remove whole objects from photographs. For more details, please visit the link to the full wikipedia article [here](https://en.wikipedia.org/wiki/Seam_carving).

### Technologies used

	* Boost
	* Python
	* GIL
	* Nuke

### Plugin structure

The source code of any plug-in consists of the following files:

* **mainEntry.cpp: ** This file has the plug-in identifiers and allow access to some files to the host software (Nuke in this case).

* **SeamCarvingDefinitions.hpp:** Contains the definitions and declarations of the visible data (input) of the plug-in. It is in this file where the buttons, switchs, slides, etc... are defined so the user can interact with the application and provide input values.

* **SeamCarvingPluginFactory.hpp:** This file has a global description of the programe, such as the type of data supported by the plug-in (image components, bit depth, tile rendering, etc.).

Contient les définitions et déclarations des données visibles du plug-in. C'est ici que l'on définira
les commandes (boutons, switch...) sur lesquels l'utilisateur peut intervenir pour modifier
facilement les paramètres entrant dans l'algorithme du plug-in.

* **SeamCarvingPlugin.hpp:** In this file the plug-in is defined as a node (of Nuke). The inputs and outputs of the node are declared.

* **SeamCarvingProcess.tcc:** This file is the heart of the program. It contains the implementation of the SeamCarving algorithm.

### Experimental results

![img1](https://s19.postimg.org/eo6xx0woj/seam_carving_results_1.png)
![img2](https://s19.postimg.org/ezoe9sd4j/seam_carving_results_2.png)