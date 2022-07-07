.. include:: ../exports/alias.include

.. _user_manual_configuration:

########################################
Fast DDS Visualizer Plugin Configuration
########################################

Once the plugin is started, a new menu appears to configure the internal |fastdds|
*Domain Participant*.
This section explains the different configuration options.
Check section :ref:`user_manual_start` to learn how to start the plugin.

Topic Selector
==============

.. figure:: /rst/figures/configuration_topic_selector_edited_circle.png

In order for |ddsvisualizer| to receive data from a DDS topic, it is required to select the DDS topics.
In each of the topics selected, the |ddsvisualizer| will create a *DataReader* and receive the data,
allowing to plot it afterwards.
The following capture shows the *topic selector* dialog.

Select topics
-------------

In this tab it is shown every
`DDS Topic <https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/topic/topic.html>`_
that is discovered by the internal *Domain Participant* in the current *Domain Id*.
Each topic name has its related data type name.
This is the data type of the data that will be published within this topic.
Be aware that some data types may not be known by the internal |fastdds| application, and must be discovered via
Dynamic Types.
Refer to the following section :ref:`dynamic_data_types_sec` for more information.

* In order to select a topic to be visualized, click on the topic name or data type name *(A)*.
* In order to deselect a topic, click it again and it will be unselected *(B)*.
* Those topics which data types have not been discovered by the application could not be selected *(C)*.
  This is because the internal |fastdds| application could not create a *DataReader* in those topics, because
  it will not be able to know the data type it is reading.

.. figure:: /rst/figures/configuration_topic_selector_edited_letters.png

Select all topics
^^^^^^^^^^^^^^^^^

In order to select all possible topics discovered, press :code:`Ctrl+A`.
In order to deselect all topics, press :code:`Ctrl+Shift+A`.

Filter topics
-------------

There is a filter in the upper right corner of the tab that allows to filter topics by name.
Writing plain text in this text box will hide those topics that do not contain such text in their names.
Be aware that those topics selected before filtering applies will still be selected even if hidden.

.. figure:: /rst/figures/configuration_topic_selector_filter.png


DDS Configuration
=================

.. figure:: /rst/figures/configuration_dds_conf_edited.png

In this tab is configured the internal *Domain Participant* of the |ddsvisualizer|.

DDS Domain Id
-------------

To select the
`DDS Domain Id <https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/domain/domain.html?highlight=Domain>`_
of the internal |fastdds| *Domain Participant*, press :code:`Change Domain` button.
This pops up a new Dialog to choose a new *Domain Id*.

.. figure:: /rst/figures/configuration_change_domain.png

XML Data Types
--------------

In order to explicitly specify a Data Type for a topic, XML files with data type descriptions could be imported.
Using the buttons :code:`Include File` or :code:`Include Directory` the user can select an XML file or all
the XML files in a directory, and they will be parsed by |fastdds| and their data types will be known for
the application hereinafter.
Check section :ref:`user_manual__dynamic_data_types__xml_data_type` for more information.


Advance Options
===============

This section is meant to configure advance options regarding internal values of data series.

.. figure:: /rst/figures/configuration_advanced_options_edited.png

Maximum array size
------------------

In some use cases, an image could be tried to display as a time series of many integer values,
which could make the application unresponsive when working with big images.
Due to very big amount of values, this could make the application highly unresponsive.
In order to avoid this, an array size limit can be set in this section.
When array data is bigger than this limit, two options can be chosen:

* :code:`discard`: this data will be discarded.
* :code:`clamp`: there will only be shown the first *N* values of the array, being *N* the maximum array size.
