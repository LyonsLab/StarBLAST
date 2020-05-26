******************************************************
StarBLAST-HPC: HPC Deployment for Large Classes (>100)
******************************************************

The StarBLAST-HPC Setup is ideal for distributing BLAST searches across multiple nodes on a High-Performance Computer.

In order to achieve a successful setup of the StarBLAST HPC system, a moderate amount of command line knowledge is required.

Similar to the StarBLAST-Dockers on Atmosphere cloud, the StarBLAST-HPC system also has a Master-Worker set-up: an atmosphere VM machine acts as the Master, and the HPC acts as the Worker. It is suggested that the Worker is set up well ahead of time.

HPC Requirements and Setup
==========================

It is important that the following software are installed on the HPC:

+ `IRODS <https://docs.irods.org/master/getting_started/installation/>`_;

+ `ncbi-blast+ version 2.6.0 or newer <ftp://ftp.ncbi.nlm.nih.gov/blast/executables/blast+/LATEST/ncbi-blast-2.9.0+-src.tar.gz>`_;

+ `CCTools version 7.0.21 or newer <https://ccl.cse.nd.edu/software/files/cctools-7.1.5-source.tar.gz>`_;

+ glibc version 2.14 or newer; 

+ Support for CentOS7.

Additionally, a CyVerse account is required.

**1.** Make both ncbi-blast+ and CCTools available in your home directory; to find out your home directory do

.. code::

   pwd

Your home directory should be something similar to

.. code::

   /home/<U_NUMBER>/<USER>/

**2.** Download the software (BLAST+ and CCTools), un-tar, and add to path using

.. code::

   wget <BLAST_URL or CCTOOLS_URL>
   tar -xvf <BLAST_repo.tar.gz or CCTOOLS_repo.tar.gz>
   export PATH=$HOME</PATH/TO/BLAST/BIN/>:$PATH
   export PATH=$HOME</PATH/TO/CCTOOLS/BIN/>:$PATH

.. note::

   CCTools only works with glibc version 2.14 or newer, confirm that your HPC has glibc version 2.14 or newer is installed or avaialbe to load (check module load or avaialable documentation). In the following examples, we assume that both glibc and BLAST+ are avaiable to be loaded through `module load`.

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

In the example above, the user already has blast installed (calls it using :code:“module load blast“). The script will submit to the HPC nodes a minimum of 1 and a maximum of 8 workers per node.

**2.** Submit the :code:`.pbs` script with 

.. code::
    
   qsub <NAME_OF_PBS>.pbs

Setting Up the Master VM on the Cloud Service
=============================================

The Master VM for StarBLAST-HPC is set up similarly to how the Master for starBLAST-Docker is set up, with the difference that the Master for starBLAST-HPC **does not require the deployment script**. 
Therefore, in order to set up the Master for starBLAST-HPC, follow the same steps as in StarBLAST-Docker **without** adding the Master deployment script. Additionally, BLAST databases need to be loaded manually onto the :code:`<DATABASE>` folder.

Once the VM is ready, either access it through ssh or by using the Web Shell ("Open Web Shell" button on your VM's page). Once inside follow the next steps.

.. note::

   **IMPORTANT: THE PATH TO THE DATABASE ON THE MASTER NEED TO BE THE SAME AS THE ONE ON THE WORKER**

To ensure both the databases on the Master VM and Worker HPC are in the same directory, on the Worker HPC go to the :code:`<DATABASE>` directory and do

.. code::

   pwd
   
Then, on your Master VM, create the directory with the same path as above

.. code::

   mkdir -p SAME/PATH/TO/HPC/DATABASE/DIRECTORY/

Now you have set up the :code:`<DATABASE>` directories but you still need the databases. Databases can be parsed manually through BLAST+'s `makeblastdb` if you have your own :code:`.fasta (or .fa, .faa, .fna)` files or you can use the same databases as StarBLAST-Docker. In order to use the latter, you need to have iRODS installed (JetStream comes with iRODS pre-installed) and a CyVerse account. Then, do:

.. code::

   iinit

It will ask for certain credentials, connect to the CyVerse with:

.. code::

   host name (DNS): data.cyverse.org
   port #: 1247
   username: <CyVerse_ID>
   zone: iplant
   password: <CyVerse_password>

If successful, obtain the databases and move them to your :code:`<DATABASE>` folder:

.. code::

   iget -rKVP /iplant/home/cosimichele/200503_Genomes_n_p
   mv GCF_* /DATABASE/DIRECTORY/
   
Then move the databases to the HPC through either :code:`sftp` or follow the same steps as above if your HPC system has access to iRODS.

Copy and paste the following code in the Master instance to launch sequenceServer.

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