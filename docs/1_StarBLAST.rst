StarBLAST
-----------

StarBlast is a scalable extension of the open source `SequenceServer <http://sequenceserver.com/>`_ BLAST, with the goal of making BLAST more accessible to educators and researchers who want to run classroom-scale searches concurrently. 
StarBLAST is distributed scaling of SequenceServer BLAST using `cctools <http://ccl.cse.nd.edu/>`_ and CyVerse's Visual Interactive Computing Environment (`VICE <https://learning.cyverse.org/projects/vice/en/latest/getting_started/about.html/>`_).


In 2015 a modern front-end implementation of BLAST, SequenceServer, was developed by the Wurmlab at Queen Mary University of London (Priyam et al., 2019). 
SequenceServer provides various advantages over the NCBI BLAST implementation, such as an improved GUI to visualize BLAST results, the use of custom databases, and the ability to download results in various formats for further analysis. Read about SequenceServer's user-centric design and sustainable software development philosophy `here <https://doi.org/10.1093/molbev/msz185>`_. 
Despite being a state-of-the-art BLAST service, the current SequenceServer implementation is not easily scalable, especially for classes without IT support or classes with hundreds of students.
StarBLAST extends SequenceServer’s BLAST implementation with the `Work Queue <https://cctools.readthedocs.io/en/latest/work_queue/>`_ job management system and `VICE <https://learning.cyverse.org/projects/vice/en/latest/getting_started/about.html/>`_ to distribute work amongst multiple machines. 
By providing distributed and scalable BLAST capabilities, StarBLAST enables researchers and instructors to run many BLAST jobs simultaneously using large computational resources. StarBLAST consists of three distinct BLAST implementations that users can select based on their technical expertise, the number of users who want to submit concurrent jobs, and external computational resource availability.


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
.. _Tut_0: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_03.png
.. |Tut_0B| image:: ./img/JS_04.png
    :width: 700
.. _Tut_0B: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_04.png
.. |Tut_1| image:: ./img/JS_02.png
    :width: 700
.. _Tut_1: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_02.png
.. |Tut_2| image:: ./img/TJS_05.png
    :width: 700
.. _Tut_2: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_05.png
.. |Tut_3| image:: ./img/JS_06.png
    :width: 700
.. _Tut_3: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_06.png
.. |Tut_4| image:: ./img/JS_07.png
    :width: 700
.. _Tut_4: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_07.png
.. |Tut_5| image:: ./img/JS_08.png
    :width: 700
.. _Tut_5: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_08.png
.. |Tut_6| image:: ./img/JS_09.png
    :width: 700
.. _Tut_6: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_09.png
.. |Tut_7| image:: ./img/JS_10.png
    :width: 700
.. _Tut_7: https://raw.githubusercontent.com/uacic/StarBlast/master/img/JS_10.png
    <a href="https://de.cyverse.org/de/" target="_blank">Discovery Environment</a>