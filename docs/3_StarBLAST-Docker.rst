***************************************************************
StarBLAST-Docker: Cloud Deployment for Medium  Classes (25-100)
***************************************************************

To deploy StarBLAST setup in a cloud provider, you will need accounts with those providers.  This example uses  `XSEDE's JetStream Cloud service <https://use.jetstream-cloud.org/>`_. You can access JetStream using an `XSEDE <https://portal.xsede.org/my-xsede#/guest>`_ account, a `Globus <https://www.globus.org/>`_ account, or via institutional access to XSEDE (search for your institution name from the drop down menu in JetStream's login page).

This setup uses a "Foreman" instance for the front-end sequenceServer and one or more "Worker" instances to distribute the computational load of running blast. Docker containers are used to deploy the Foreman and Workers through deployment scripts. These deployment scripts are designed to:

  + Get appropriate  `Worker <https://hub.docker.com/r/zhxu73/sequenceserver-scale-worker>`_ and `Foreman <https://hub.docker.com/r/zhxu73/sequenceserver-scale>`_ containers
  + Download specified BLAST databases (can customize)
  + Launch sequenceServer front-end that can be accessed using the code: `<FOREMAN_IP_ADDRESS>` of the instance
  + Connect factory of workers to the Foreman

Launching Foreman & Worker Instances
===================================

**1.**  Login to `JetStream Cloud <https://use.jetstream-cloud.org/>`_. 


**2.** From `JetStream's <https://use.jetstream-cloud.org/application/dashboard>`_ top menu, navigate to "Projects" and select "Create New Project".

|Tut_0|_

**3.** In the "Project Name" field, name your project and add a description.

|Tut_0B|_

**4.** From `JetStream's <https://use.jetstream-cloud.org/application/dashboard>`_ dashboard, select “Launch New Instance”.

|Tut_1|_

**5.** Be sure to change the default tab from "Show Featured" to "Show All", search for “Docker_starBLAST” and select the “Docker_starBLAST” image (or click `here <https://use.jetstream-cloud.org/application/images/967>`_); click “Launch”.

|Tut_2|_

|Tut_3|_ 

**6.** In the pop up menu you can customize your image (e.g. Instance Size. Use a minimum of m1.xlarge instance for Foreman, with at least 60GB disk space); select “Advanced Options”.

|Tut_4|_

**7.** Select “Create a New Script”. 

|Tut_5|_

**8.**  Title the script "Foreman" or similar, select “Raw Text” and copy and paste the Foreman script, linked below. The scripts generate a password and username based on the user account, but these can be personalized if needed (*not suggested for new users*). Select “Save and Add Script” and then "Continue to Launch".

**Deployment Scripts**

+ The deployment scripts for a *Foreman instance (atmo_deploy_master.sh)* can be found `here <https://raw.githubusercontent.com/zhxu73/sequenceserver-scale-docker/master/deploy/iRODS/Jetstream_deploy_master.sh>`_.
+ The deployment scripts for a *Worker instance (atmo_deploy_worker.sh)* can be found `here <https://raw.githubusercontent.com/zhxu73/sequenceserver-scale-docker/master/deploy/iRODS/Jetstream_deploy_worker.sh>`_.

.. note::
   This step is required to be done **once** for the Foreman and **once for each Worker instance**. The deployment scripts are stored for future use.

|Tut_6|_

**9.** Repeat steps 4-8 for one or more Worker instance(s), using the Worker deployment script. Use large or extra large images (at least 60GB of disk space is required).

.. note::
   JetStream cloud will take at least 10-20 minutes and the wait-time will increase with the size of the BLAST database.


Start BLASTING! Now anyone can enter the :code:`<FOREMAN_IP_ADDRESS>` into their browser and access SequenceServer.

|Tut_7|_


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
.. _Tut_0: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_03.png
.. |Tut_0B| image:: ./img/JS_04.png
    :width: 700
.. _Tut_0B: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_04.png
.. |Tut_1| image:: ./img/JS_02.png
    :width: 700
.. _Tut_1: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_02.png
.. |Tut_2| image:: ./img/JS_05.png
    :width: 700
.. _Tut_2: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_05.png
.. |Tut_3| image:: ./img/JS_06.png
    :width: 700
.. _Tut_3: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_06.png
.. |Tut_4| image:: ./img/JS_07.png
    :width: 700
.. _Tut_4: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_07.png
.. |Tut_5| image:: ./img/JS_08.png
    :width: 700
.. _Tut_5: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_08.png
.. |Tut_6| image:: ./img/JS_09.png
    :width: 700
.. _Tut_6: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_09.png
.. |Tut_7| image:: ./img/JS_10.png
    :width: 700
.. _Tut_7: https://github.com/LyonsLab/StarBLAST/tree/master/docs/img/JS_10.png
    <a href="https://de.cyverse.org/de/" target="_blank">Discovery Environment</a>
