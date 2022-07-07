.. raw:: html

  <h1>
    Fast DDS Visualizer Plugin Documentation
  </h1>

.. image:: /rst/figures/logo.png
  :height: 100px
  :width: 100px
  :align: left
  :alt: eProsima
  :target: http://www.eprosima.com/

*eProsima Fast DDS Visualizer Plugin* is a plugin for the *PlotJuggler* application.
PlotJuggler is a graphical desktop application providing visualizations features 
of data series, time series, X-Y plots, as well as data management features as 
data import and export, custom and built-in data manipulation functions,
data series merges, etc.
As well, this software support many different layouts, with dynamic and user-friendly customization.

*Fast DDS Visualizer Plugin* allows users to visualize topic-related data from a DDS network.
The user can select several topics from the discovered topics running in the DDS network.
These topics will be divided by values using data type introspection,
and each value could be visualized and managed separately,
showing the user a quick and rich representation of the detailed data content that is being exchanged in the network.

*eProsima Fast DDS Visualizer Plugin* supports the following features:

#. *Data type introspection*: by using *Fast DDS Dynamic Types*,
    the plugin allows to discover the data type of the topic,
    and to visualize the data content using the corresponding data type.

#. *DDS Configuration*: Different DDS configurations are allowed as choosing the *Domain Id* and selecting
    the *DDS Topics* to be visualized.

#. *All PlotJuggler features*: This plugin makes it possible to use the huge amount of features that PlotJuggler
    offers to visualize and manage data from a DDS network.

.. raw:: html

    <video width=100% height=auto autoplay loop controls>
        <source src="../../_static/usage_example.mp4">
        Your browser does not support the video tag.
    </video>



###############################
Contacts and Commercial support
###############################

Find more about us at `eProsima's webpage <https://eprosima.com/>`_.

Support available at:

* Email: support@eprosima.com
* Phone: +34 91 804 34 48

#################################
Contributing to the documentation
#################################

*DDS Visualizer Plugin Documentation* is an open source project, and as such all contributions, both in the form of
feedback and content generation, are most welcomed.
To make such contributions, please refer to the
`Contribution Guidelines <https://github.com/eProsima/all-docs/blob/master/CONTRIBUTING.md>`_ hosted in our GitHub
repository.

##############################
Structure of the documentation
##############################

This documentation is organized into the sections below.

* :ref:`Installation Manual <installation_manual_linux>`
* :ref:`Getting Started <start_tutorial>`
* :ref:`User Manual <user_manual_start>`
* :ref:`Developer Manual <developer_manual_installation_sources_linux>`
* :ref:`Release Notes <release_notes>`
