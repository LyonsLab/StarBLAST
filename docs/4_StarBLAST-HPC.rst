******************************************************
StarBLAST-HPC: HPC Deployment for Large Classes (>100)
******************************************************

The StarBLAST-HPC Setup is designed to distribute BLAST searches across multiple nodes on a High-Performance Computer and uses a Master-Worker set-up similar to StarBLAST-Docker (an atmosphere instance as the Master, and the HPC as the Worker). It is suggested that the Worker is set up ahead of time.

Some command line knowledge is required for setup.


HPC Requirements and Setup
==========================

It is important that the following software are installed on the HPC:

+ `IRODS <https://docs.irods.org/master/getting_started/installation/>`_;

+ `ncbi-blast+ version 2.9.0 or newer <ftp://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/LATEST/ncbi-blast-2.9.0+-src.tar.gz>`_;

+ `CCTools version 7.0.21 or newer <https://ccl.cse.nd.edu/software/files/cctools-7.1.5-source.tar.gz>`_;

+ glibc version 2.14 or newer; 

+ Support for CentOS7.

Additionally, a CyVerse account is required.

**1.** Make both ncbi-blast+ and CCTools available in your home directory, which can be found using

.. code::

   pwd

It should output something similar to

.. code::

   /home/<U_NUMBER>/<USER>/

**2.** Download the software (BLAST+ and CCTools), un-tar, and add to path using

.. code::

   wget <BLAST_URL or CCTOOLS_URL>
   tar -xvf <BLAST_repo.tar.gz or CCTOOLS_repo.tar.gz>
   export PATH=$HOME</PATH/TO/BLAST/BIN/>:$PATH
   export PATH=$HOME</PATH/TO/CCTOOLS/BIN/>:$PATH

.. note::

   CCTools only works if your HPC has glibc version 2.14 or newer. In the following examples, glibc and BLAST+ are loaded through `module load`.

**3.** BLAST databases need to be downloaded in a <DATABASE> directory in the home folder.

.. code::

   /home/<U_NUMBER>/<USER>/<DATABASE>

Launching Workers on the HPC
============================

The HPC uses a .pbs and qsub system to submit jobs.

**1.** Create a :code:`.pbs` file that contains the following code and change the :code:`<VARIABLES>` to preferred options:

.. code::

   #!/bin/bash
   #PBS -W group_list=<GROUP_LIST>
   #PBS -q windfall
   #PBS -l select=<N_OF_NODES>:ncpus=<N_OF_CPUS>:mem=<N_MEMORY>gb
   #PBS -l place=pack:shared
   #PBS -l walltime=<MAX_TIME>
   #PBS -l cput=<MAX_TIME>
   module load blast
   module load unsupported
   module load ferng/glibc
   module load singularity
   export CCTOOLS_HOME=/home/<U_NUMBER>/<USER>/<CCTOOLS_DIRECTORY>
   export PATH=${CCTOOLS_HOME}/bin:$PATH

   cd /home/<U_NUMBER>/<USER>/<WORKERS_DIRECTORY>

   MASTER_IP=<MASTER_IP>
   MASTER_PORT=<PORT_NUMBER>
   TIME_OUT_TIME=<TIME_OUT_TIME>
   PROJECT_NAME=<PROJECT_NAME>

   /home/<U_NUMBER>/<USER>/<CCTOOLS_DIRECTORY>/bin/work_queue_factory -T local -M $PROJECT_NAME --cores <N_CORES> -w <MIN_N_WORKERS> -W <MAX_N_WORKERS> -t $TIME_OUT_TIME

An example of a :code:`.pbs` file running on the University of Arizona HPC:

.. code::

   #!/bin/bash
   #PBS -W group_list=lyons-lab
   #PBS -q windfall
   #PBS -l select=2:ncpus=12:mem=24gb
   #PBS -l place=pack:shared
   #PBS -l walltime=02:00:00
   #PBS -l cput=02:00:00
   module load blast
   module load unsupported
   module load ferng/glibc
   module load singularity
   export CCTOOLS_HOME=/home/u12/cosi/cctools-7.0.19-x86_64-centos7
   export PATH=${CCTOOLS_HOME}/bin:$PATH

   cd /home/u12/cosi/cosi-workers

   MASTER_IP=128.196.142.13
   MASTER_PORT=9123
   TIME_OUT_TIME=1800
   PROJECT_NAME="starBLAST"

   /home/u12/cosi/cctools-7.0.19-x86_64-centos7/bin/work_queue_factory -T local -M $PROJECT_NAME --cores 12 -w 1 -W 8 -t $TIME_OUT_TIME

In the example above, the user already has blast installed (calls it using :code: “module load blast“). The script will submit to the HPC nodes a minimum of 1 and a maximum of 8 workers per node.

**2.** Submit the :code:`.pbs` script with 

.. code::
    
   qsub <NAME_OF_PBS>.pbs

Setting Up the Master VM on the Cloud Service
=============================================

Set up the Master instance for starBLAST-HPC by following the same steps as for StarBLAST-Docker, but **without adding the Master deployment script**. Additionally, BLAST databases need to be loaded manually onto the :code:`<DATABASE>` folder.

Once the VM is running, access it through ssh or by using the Web Shell ("Open Web Shell" button on your VM's page). Once inside follow the next steps.

.. note::

   **IMPORTANT: THE PATH TO THE DATABASE ON THE MASTER NEED TO BE THE SAME AS THE ONE ON THE WORKER**


**1.** Ensure the databases on both the Master VM and Worker HPC are in the same directory. On the Worker HPC go to the :code:`<DATABASE>` directory and do

.. code::

   pwd
   
Then, on your Master VM, create the directory with the same path output above

.. code::

   mkdir -p SAME/PATH/TO/HPC/DATABASE/DIRECTORY/

**2.** Now the :code:`<DATABASE>` directories have been set up to contain the desired databases. You can use the same databases preset for StarBLAST-Docker or make your own from a :code:`.fasta (or .fa, .faa, .fna)` file using BLAST+'s `makeblastdb` referenced in StarBLAST-VICE. Both require iRODS (JetStream comes with iRODS pre-installed) and a CyVerse account. 

Access iRODS using:

.. code::

   iinit

You will be prompted to connect to the CyVerse with:

.. code::

   host name (DNS): data.cyverse.org
   port #: 1247
   username: <CyVerse_ID>
   zone: iplant
   password: <CyVerse_password>

**3.** Once connected, retreive and move the databases to your :code:`<DATABASE>` folder (shown for preset):

.. code::

   iget -rKVP /iplant/home/cosimichele/200503_Genomes_n_p
   mv GCF_* /DATABASE/DIRECTORY/
   
**4.** Move the databases to the HPC using either :code:`sftp` or the steps as above if your HPC system has iRODS.

**5.** Use this code within the Master instance to launch sequenceServer:

.. code:: 

   docker run --rm --name sequenceserver-scale -p 80:3000 -p 9123:9123 -e PROJECT_NAME=<PROJECT_NAME> -e WORKQUEUE_PASSWORD=<PASSWORD> -e BLAST_NUM_THREADS=<N THREADS> -e SEQSERVER_DB_PATH="/home/<U_NUMBER>/<USER>/<DATABASE_DIRECTORY>" -v /DATABASE/ON/MASTER:/DATABASE/ON/WORKER zhxu73/sequenceserver-scale:no-irods
   
An example is:

.. code:: 

   docker run --rm --name sequenceserver-scale -p 80:3000 -p 9123:9123 -e PROJECT_NAME=starBLAST -e WORKQUEUE_PASSWORD= -e BLAST_NUM_THREADS=2 -e SEQSERVER_DB_PATH="/home/u12/cosi/DATABASE" -v /home/u12/cosi/DATABASE:/home/u12/cosi/DATABASE zhxu73/sequenceserver-scale:no-irods
   
.. note::

   The custom Database folder on the Master needs to have read and write permissions
   
Start BLASTING! Now anyone can enter the :code:`<MASTER_VM_IP_ADDRESS>` in their browser using to access SequenceServer.


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
