.. StarBLAST documentation master file, created by
   sphinx-quickstart on Thu May 21 12:03:50 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to StarBLAST!
=====================

|starblast_logo|_

StarBLAST is a scalable extension of the open source `SequenceServer <http://sequenceserver.com/>`_ BLAST implementation, with the goal of making BLAST more accessible to researchers and educators who want to run classroom-scale searches concurrently. 
StarBLAST allows SequenceServer to scale using `CCTOOLS <http://ccl.cse.nd.edu/>`_, a collection of programs that enable jobs to be distributed over a network. 
Furthermore, we developed a StarBLAST application hosted on the CyVerse's Visual Interactive Computing Environment (`VICE <https://learning.cyverse.org/projects/vice/en/latest/getting_started/about.html/>`_) for quickly accessing a personalizable SequenceServer app. 


What is SequenceServer?
=======================

In 2015 a modern front-end implementation of BLAST, SequenceServer, was developed by the Wurmlab at Queen Mary University of London (Priyam et al., 2019). 
SequenceServer provides various advantages over the NCBI BLAST implementation, such as an improved GUI to visualize BLAST results and the ability to use custom databases. 
Read more about SequenceServer's user-centric design and sustainable software development philosophy `here <https://doi.org/10.1093/molbev/msz185>`_. 
Despite being a state-of-the-art BLAST service, the current SequenceServer implementation is not easily scalable, especially for classes without IT support or classes with hundreds of students.

The StarBLAST Suite
===================

Navigate to each implementation for  information on guided deployment:

+ `StarBLAST-VICE <https://starblast.readthedocs.io/en/latest/2_StarBLAST-VICE.html>`_
+ `StarBLAST-Docker <https://starblast.readthedocs.io/en/latest/3_StarBLAST-Docker.html>`_
+ `StarBLAST-HPC <https://starblast.readthedocs.io/en/latest/4_StarBLAST-HPC.html>`_

.. |seqserver_QL| image:: https://de.cyverse.org/Powered-By-CyVerse-blue.svg
.. _seqserver_QL: https://de.cyverse.org/de/?type=quick-launch&quick-launch-id=0ade6455-4876-49cc-9b37-a29129d9558a&app-id=ab404686-ff20-11e9-a09c-008cfa5ae621

.. |concept_map| image:: ./img/concept_map.png
    :width: 700
.. _concept_map: 

.. |CyVerse logo| image:: ./img/cyverse_rgb.png
    :width: 700
.. _CyVerse logo: http://learning.cyverse.org/
.. |Home_Icon| image:: ./img/homeicon.png
    :width: 25
.. _Home_Icon: http://learning.cyverse.org/
.. |starblast_logo| image:: ./img/starblast.jpeg
    :width: 700
.. _starblast_logo:   
.. |discovery_enviornment| raw:: html
.. |Tut_0| image:: ./img/JS_03.png
    :width: 700