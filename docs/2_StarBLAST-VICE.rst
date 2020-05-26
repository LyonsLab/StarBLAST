******************************************************
StarBLAST-VICE: Web Deployment for Small Classes (<25)
******************************************************

StarBLAST-VICE is a personalizable implementation of SequenceServer, deployable as a VICE (Visual and Interactive Computing Environment) web application and hosted on the CyVerse’s Discovery Environment (DE).
StarBLAST-VICE is launchable with a maximum of 8 CPU cores, 16 GB RAM, and 512 GB disk space.

.. note::

   Before proceeding, a CyVerse account is required. Click `here <https://de.cyverse.org/de/>`_ to register or log in. 

Launching StarBLAST-VICE with Example Databases
===============================================

**1.** Click on the following button to launch SequenceServer in CyVerse Discovery Environment with two BLAST databases (Human_GRCh38_p12 & Mouse_GRCm38_p4 - **requires CyVerse account**).

	|seqserver_QL|_
	

**2.** Choose your own analysis name and the DE output folder. Click "Launch Analysis".


**3.** Check the notifications Bell Icon for a link to access your SequenceServer instance. This might take a few minutes depending on the amount of resources requested.


**4.** Click on the notification to open the SequenceServer instance in a new tab. Click `here <https://www.ncbi.nlm.nih.gov/nuccore/NG_007114.1?from=4986&to=6416&report=fasta>`_ for a sample DNA sequence to test the sequence similarity of the query fragment with random human and mouse sequences.


**5.** Paste the query sequence and select both the available databases and submit job.

Adding Your Own Databases to StarBLAST-VICE
===========================================

To add your own BLAST databases you will need a :code:`.fasta (or .fa, .faa, .fna)`  file of your organism of choice. These are easily aquirable from NCBI or other databases.

**1.** Click on the "Data" button. 


**2.** Upload or import your fasta file to the CyVerse DE (click on "Upload" and choose whether to uplodad from your computer or insert the fasta URL). The sequence will be stored in your own private folder.


**3.** Click on the "Search Apps" bar and search for "Create BLAST Database" or click `here <https://de.cyverse.org/de/?type=apps&app-id=decdd668-5616-11e7-9724-008cfa5ae621&system-id=de>`_. 


**4.** In the "Inputs" tab, choose the fasta file you uploaded. Additionally, add a name for your BLAST database and choose a mew destination folder.


**5.** Click "Launch Analysis" and wait for the notification bell to notify you when the BLAST database is finished creating.

Launching StarBLAST-VICE with Your Own Databases
================================================

You have now created your own BLAST database. From within the CyVerse DE, to launch StarBLAST-VICE with your own database do:

**1.** Access the SequenceServer app as above, but **do not click "Launch Analysis" just yet**.


**2.** Navigate to the "Input" tab and choose the folder where you saved the BLAST database you created.


**3.** Proceed by clicking "Launch Analysis". As before, this might take a few minutes depending on the amount of resources requested.




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
.. _Tut_0: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_03.png
.. |Tut_0B| image:: ./img/JS_04.png
    :width: 700
.. _Tut_0B: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_04.png
.. |Tut_1| image:: ./img/JS_02.png
    :width: 700
.. _Tut_1: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_02.png
.. |Tut_2| image:: ./img/TJS_05.png
    :width: 700
.. _Tut_2: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_05.png
.. |Tut_3| image:: ./img/JS_06.png
    :width: 700
.. _Tut_3: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_06.png
.. |Tut_4| image:: ./img/JS_07.png
    :width: 700
.. _Tut_4: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_07.png
.. |Tut_5| image:: ./img/JS_08.png
    :width: 700
.. _Tut_5: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_08.png
.. |Tut_6| image:: ./img/JS_09.png
    :width: 700
.. _Tut_6: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_09.png
.. |Tut_7| image:: ./img/JS_10.png
    :width: 700
.. _Tut_7: https://github.com/uacic/StarBlast/tree/master/docs/img/JS_10.png
    <a href="https://de.cyverse.org/de/" target="_blank">Discovery Environment</a>