---
title: 'StarBLAST: a scalable BLAST+ solution for the classroom'
tags:
  - Distributed computing
  - Containers
  - BLAST
  - Genetics
  - Education
  - tectonics
authors:
 - name: Michele Cosi
   orcid: 
   affiliation: "1"
 - name: J.J. Forstedt
   orcid: 
   affiliation: "1"
- name: Emmanuel Gonzalez
   orcid: 
   affiliation: "1"
- name: Zhuoyun Xu
   orcid: 
   affiliation: "1"
- name: Sateesh Peri
   orcid: 
   affiliation: "1"   
- name: Reetu Tuteja
   orcid: 
   affiliation: "1"
- name: Kai Blumberg
   orcid: 
   affiliation: "1"
- name: Tanner Campbell
   orcid: 
   affiliation: "1"
- name: Nirav Merchant
   orcid: 
   affiliation: "1"
- name: Eric Lyons
   orcid: 
   affiliation: "1"

affiliations:
 - name:  - name: Data Sciences Institute, University of Arizona, Tucson, AZ 85721, USA
   index: 1

date: 14 August 2020
bibliography: citations.bib
---



**Availability:** StarBLAST source code is available: https://github.com/uacic/StarBlast; Docker Images -- Worker: https://dockr.ly/3a4G3kf; Master: https://dockr.ly/38JCzDg; StarBLAST CyVerse app: http://bit.ly/32dSpDI; Documentation: https://starblast.readthedocs.io/en/latest/


**Contact:** ericlyons@email.arizona.edu

# Summary

BLAST [@madden2002] is a heuristic nucleotide and protein search tool for matching query sequences to a specified genetic database. Frequently used in biological research, the use of BLAST for instructional purposes is becoming increasingly common in classrooms, including the high school-grade level [@form2011]. While NCBI hosts a publicly accessible version of BLAST, NCBI-BLAST+, it may be problematic in a classroom environment: many job submissions originating from the same IP address are likely to be throttled (IP block); there is no option to input custom databases; and response time may be slow. In addition, databases hosted at NCBI are frequently updated as new sequences become available, which may change the results from when lesson plans are designed and cause confusion for students. SequenceServer [@priyam2019] addresses some of these needs by providing an implementation of BLAST+ that has a student-friendly web-accessible interface, enables instructors to easily use custom databases, and has an improved interface for teaching. While BLAST+ has images available for use in Amazon Web Services, it takes considerable expertise to use and has additional monetary costs. SequenceServer installation is simpler compared to a BLAST+ local installation by either using Ruby install packages or Docker containers. However, SequenceServer does not address the scalability issues for large class sizes and deploying the software is still difficult for classrooms with minimal computational resources and technical expertise. To address these needs, we developed StarBLAST, which has a modular deployment strategy for SequenceServer that accounts for a variety of technical expertise and classroom sizes. This is achieved by leveraging a master-worker framework for distributed and scalable computing using WorkQueue and Makeflow [@albrecht2012]. WorkQueue manages jobs (compute tasks) to be distributed from a “Master” node to designated “Worker” nodes for completion. Its inherent scalability makes it a solution for large classrooms. For smaller classrooms, often a single compute node is sufficient, and we integrated SequenceServer in CyVerse’s Visual Interactive Computing Environment (VICE). Through VICE, an instructor can quickly deploy SequenceServer and databases through a web-based interface. Here, we describe the three deployable methods of StarBLAST that allow educators to use custom databases for BLAST and select their deployment based on their classroom size, technical expertise, and computational resources available.

# Description

StarBLAST methods include StarBLAST-VICE, StarBLAST-Docker, and StarBLAST-HPC. StarBLAST-VICE is hosted in the CyVerse Discovery Environment (DE) [@merchant2016], a web-based data science workbench for researchers. StarBLAST-Docker and StarBlast-HPC integrate SequenceServer with WorkQueue and Makeflow for distributing BLAST jobs across multiple computer nodes on the Cloud or high-performance computing (HPC) environments, respectively.

## StarBLAST-VICE

StarBLAST-VICE is a dockerized image of SequenceServer integrated as a VICE application in the CyVerse DE. This solution quickly launches SequenceServer on a virtual machine with up to 8 CPU cores and 16GB RAM, and is practical for small classrooms (<25 students). Launching StarBLAST-VICE requires minimal technical expertise and no supporting infrastructure except for an internet connection, and custom BLAST databases may be specified. A supporting app, *Create_BLAST_database*, is integrated in CyVerse to allow instructors to quickly create custom BLAST databases. The deployment speed of the StarBlast-VICE app is dependent on the size of the input BLAST database, which is copied to the VM, and usually takes only a few minutes.

## StarBLAST-Docker

StarBLAST-Docker uses WorkQueue and Makelfow for a master-worker framework to distribute BLAST jobs among a master Virtual Machine (VM) and one (or more) Worker VMs allowing for scalability. Although intended for use on NSF’s XSEDE Jetstream (Stewart, C., *et al*., 2015, StarBLAST-Docker is compatible with other cloud computational resources such as Digital Ocean and Google Cloud. StarBLAST-Docker requires two or more VMs to be run and can scale to handle a moderate number of students (<100, depending on the number of worker VMs). The master VM is responsible for managing SequenceServer and sending BLAST jobs to worker VMs. Once these VMs are started and configured, students can easily connect to SequenceServer on the master VM using a web-browser. Setting up StarBLAST-Docker requires minimal experience for managing VMs and command-line. Prior to launching the VMs, two deployment scripts need to run, one each for the master and worker VMs. The deployment scripts are available in GitHub and a step-by-step tutorial is available in the StarBLAST documentation. BLAST databases are automatically downloaded to each worker VM from the CyVerse DataStore, and custom databases can be indexed by either using the companion app *Create_BLAST_database* or NCBI’s *makeblastdb* command (available with SequenceSever) and stored in the same location as other databases on all of the VMs. StarBLAST-Docker is scalable depending on the size and number of worker VMs provided by the user.An additional option for StarBLAST-Docker is its ability for deployment across a local area network using the instructor’s computer as the master and the students’ computers as workers. This method keeps everything local and is ideal for classroom environments with limited or unreliable internet connectivity.

## StarBLAST-HPC

StarBLAST-Docker workers can be deployed on HPC systems instead of VMs, enabling further scalability. Setting up StarBLAST-HPC requires moderate knowledge of the linux command-line, installing linux tools, and submitting jobs on an HPC (StarBLAST’s documentation has examples for PBS-Pro.) The master node for StarBLAST-HPC, equivalent to the master VM as StarBlast-Docker, is deployed to manage SequenceServer and send BLAST jobs to HPC worker nodes on. Workers run on nodes of the HPC using WorkQueue’s *work_queue_factory*, allowing instructors to specify the number of workers to be run on each node based on the amount of resources available per node. Given that many HPC systems have nodes with 16-96 cores, the number can be tailored to optimize the number of workers needed versus the current workload on the HPC. Since HPC resources are shared and may not be available instantly, HPC submission requests must be done well ahead of class to ensure the worker nodes are available and accessible. Due to the computational power of the HPC, a large number of students can be supported by StarBLAST-HPC (>100). The script and HPC set-up tutorials are available in StarBLAST’s documentation. BLAST databases are obtainable from the CyVerse DataStore but are added manually to the master VM; the instructor/IT staff are required to copy the BLAST databases to the HPC. 

**Availability and Requirements**

StarBLAST-VICE is available on CyVerse’s Discovery Environment (https://de.cyverse.org/de/). StarBLAST-Docker Master and Worker VM images are available on NSF’s XSEDE Jetstream (https://use.jetstream-cloud.org/). Deployment scripts can be obtained from [https://github.com/uacic/StarBlast/](https://github.com/uacic/StarBlast). To set up StarBLAST-Docker on a different cloud computational resource, Docker version 19.03.5 and CCTools version 7.0.21 are required. HPC integration requires access to a High Performance Computing system. Documentation to the StarBLAST suite is available at https://starblast.readthedocs.io/en/latest/. Instructors and students may need to create a CyVerse account to access StarBLAST-VICE and an XSEDE account to access StarBLAST-Docker and HPC.

**Acknowledgements**

The Fall 2019 Applied Concepts in Cyberinfrastructure class at the University of Arizona. Benjamin Tovar, Douglas Thain and the CCTools team, the Sequenceserver team, Wilson Leung and the Genomic Education Alliance. 

**Funding**

This work was supported by the National Science Foundation [grant numbers IOS–1743442, IOS–1444490].
*Conflict of Interest:* none declared.
