.. include:: ../exports/alias.include

.. _user_manual_configuration:

###################################
DDS Visualizer Plugin Configuration
###################################

Once the plugin is started, a new dialog will open where choosing the configuration for the internal |fastdds|
*Domain Participant*.
This section explains the different configuration options.

.. todo::
    Add a link to the start plugin


Topic Selector
==============

.. todo::
    Add screenshot of Topic Selector marking tab

In order for |ddsvisualizer| to receive data from a DDS topic, it is required to select the DDS topics.
In each of the topics selected, the |ddsvisualizer| will create a *DataReader* and receive the data,
allowing to plot it afterwards.
The following capture shows the *topic selector* dialog.

Select topics
-------------

In this tab will automatically appear every
`DDS Topic <https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/topic/topic.html>`_
that is discovered by the internal *Domain Participant* in the current *Domain Id*.
With each topic name, it appears its data type name.
This is the data type of the data that will be published within this topic.
Be aware that some data types may not be known by the internal |fastdds| application, and must be discovered via
Dynamic Types.
Refer to the following section :ref:`dynamic_data_types_sec` for more information.

* In order to select a topic to be visualized, click on the topic name or data type name *(A)*.
* In order to deselect a topic, click it again and it will be unselected *(B)*.
* Those topics which data types have not been discovered by the application could not be selected *(C)*.
  This is because the internal |fastdds| application could not create a *DataReader* in those topics, because
  it will not be able to know the data type it is reading.

.. todo::
    Add screenshot of Topic Selector with at least 3 topics. 1 selected, 1 unselected and 1 disabled

Select all topics
^^^^^^^^^^^^^^^^^

In order to select all possible topics discovered, press :code:`Ctrl+A`.
In order to deselect all topics, press :code:`Ctrl+Shift+A`.

Filter topics
-------------

There is a filter in the upper right corner of the tab that allows to filter topics by name.
Writing plain text in there will hide those topics that do not contain such text in their names.
Be aware that those topics that are hidden will still be selected, if they were before filtering.

.. todo::
    Add screenshot of Topic Selector marking filter bar


DDS Configuration
=================

.. todo::
    Add screenshot of DDS Configuration marking tab

In this tab is configured the internal *Domain Participant* of the |ddsvisualizer|.

DDS Domain Id
-------------

To select the
`DDS Domain Id <https://fast-dds.docs.eprosima.com/en/latest/fastdds/dds_layer/domain/domain.html?highlight=Domain>`_
of the internal |fastdds| *Domain Participant*, press :code:`Change Domain` button.
This pops up a new Dialog to choose a new *Domain Id*.

.. todo::
    Add screenshot of Change DomainId dialog

XML Data Types
--------------

In order to explicitly specify a Data Type for a topic, XML files with data type descriptions could be uploaded.
Using the buttons :code:`Include File` or :code:`Include Directory` the user can select an XML file or all
the XML files in a directory, and they will be parsed by |fastdds| and their data types will be known for
the application hereinafter.
Check section :ref:`user_manual__dynamic_data_types__xml_data_type` for more information.


Advance Options
===============

This section is meant to configure advance options regarding internal values of data series.

.. todo::
    Add screenshot of Advance Options marked

Maximum array size
------------------

There is a typical use case where an image is trying to be displayed as a time series of integers.
Due to very big amount of values, this could make the application highly unresponsive.
In order to avoid so, there is a maximum size of arrays, set in this section.
There are two possibilities to follow when a data with an array bigger than that value is found:

* :code:`discard`: this data will be discarded.
* :code:`clamp`: there will only be shown the first *N* values of the array, being *N* the maximum array size.
