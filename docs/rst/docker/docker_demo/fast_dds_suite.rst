.. _fast_dds_suite:

Fast DDS Suite
==============

This Docker image contains the complete Fast DDS suite. This includes:

- :ref:`eProsima Fast DDS libraries and examples <fast_dds_suite_examples>`: *eProsima Fast DDS* is a C++
  implementation of the `DDS (Data Distribution Service) Specification <https://www.omg.org/spec/DDS/About-DDS/>`__,
  a protocol defined by the `Object Management Group (OMG) <https://www.omg.org/>`__.
  The *eProsima Fast DDS* library provides both an Application Programming Interface (API) and a communication protocol
  that deploy a Data-Centric Publisher-Subscriber (DCPS) model, with the purpose of establishing efficient and reliable
  information distribution among Real-Time Systems. *eProsima Fast DDS* is predictable, scalable, flexible, and
  efficient in resource handling.

  This Docker Image contains the Fast DDS libraries bundled with several examples that showcase a variety of
  capabilities of eProsima's Fast DDS implementation.

  You can read more about Fast DDS on the `Fast DDS documentation page <https://fast-dds.docs.eprosima.com/en/latest/>`_.

- :ref:`Shapes Demo <fast_dds_suite_shapes_demo>`: eProsima Shapes Demo is an application in which Publishers and
  Subscribers are shapes of different colors and sizes moving on a board. Each shape refers to its own topic: Square,
  Triangle or Circle. A single instance of the eProsima Shapes Demo can publish on or subscribe to several topics at
  a time.

  You can read more about this application on the `Shapes Demo documentation page <https://eprosima-shapes-demo.readthedocs.io/>`_.

- :ref:`Fast DDS Monitor <fast_dds_suite_monitor>`: eProsima Fast DDS Monitor is a graphical desktop application aimed
  at monitoring DDS environments deployed using the *eProsima Fast DDS* library. Thus, the user can monitor in real
  time the status of publication/subscription communications between DDS entities. They can also choose from a wide
  variety of communication parameters to be measured (latency, throughput, packet loss, etc.), as well as record and
  compute in real time statistical measurements on these parameters (mean, variance, standard deviation, etc.).

.. (TODO) complete
