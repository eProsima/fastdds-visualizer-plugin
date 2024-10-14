.. include:: ../exports/alias.include

.. _dynamic_data_types_sec:

########################################
Configure Dynamic Data Types in Fast DDS
########################################

|ddsvisualizer| uses
`Dynamic Types <https://fast-dds.docs.eprosima.com/en/latest/fastdds/dynamic_types/dynamic_types.html>`_ in order
to be able to introspect the data type and read the internal data.
If the application does not know the data type, it will not be able to connect to such topic.
There are two ways of retrieving the data type information to the application:

.. _user_manual__dynamic_data_types__automatic_data_type:

********************************
Automatically discover Data Type
********************************

In order for |ddsvisualizer| to discover automatically the data type of a topic,
the |fastdds| publisher that may interact with the application must be configured to send this information.
There are two main configurations that have to be considered:


Generate DataType
=================

In order for |fastdds| to be able to retrieve the data type description, it must have the TypeInformation.
In case that the |fastdds| application is already using the *Dynamic Types* API, this TypeInformation will be
already available for the :code:`DomainParticipant`.

Otherwise, if the data type is generated via
`Fast DDS Gen <https://fast-dds.docs.eprosima.com/en/latest/fastddsgen/introduction/introduction.html>`_,
it is required to generate it with option :code:`-typeobject`.
This generates two auxiliary :code:`cpp` files that include the type information of the data type.
Then, the data must be registered in the |fastdds| application before creating the publisher,
using the function provided in :code:`<DataTypeName>TypeObject.hpp` file :code:`register<DataTypeName>Types()`.

.. code-block:: cpp

    // In case the DataType is HelloWorld, the HelloWorldTypeObject.hpp will define a function
    // called registerHelloWorldTypes() that registers the data type in the whole the process.
    registerHelloWorldTypes();


Publish Type Information
========================

In order for the *DomainPartipant* in the |fastdds| application to be able to retrieve the data type information,
it requires to be configured as a
`TypeLookup Server <https://fast-dds.docs.eprosima.com/en/latest/fastdds/dynamic_types/discovery.html?highlight=typelookup#typelookup-service>`_.
To do so, add the following configuration to the *Domain Participant QoS*:

.. code-block:: cpp

    // Set DomainParticipantQoS as a TypeLookup Server
    DomainParticipantQos qos;
    qos.wire_protocol().builtin.typelookup_config.use_server = true;

Even when it is configured by default, make sure the :code:`auto_fill_type_information` value of the
:code:`TypeSupport` is :code:`true`.


.. _user_manual__dynamic_data_types__xml_data_type:

*************
XML Data Type
*************

If the automatic discovery of the data type is not possible, an XML file with the data type description could be loaded,
so the internal |ddsvisualizer| is able to know the data type to subscribe to the topic.
This XML should be added as a DDS configuration in the following tab when starting the plugin.

.. todo:

    Add link to the configuration section


XML Data Type format
====================

The format and types supported by |fastdds| are described in the
`following section <https://fast-dds.docs.eprosima.com/en/latest/fastdds/xml_configuration/dynamic_types.html#xmldynamictypes>`_
of the Fast DDS documentation.

XML Data Type example
=====================

In the following snippet, there is an example of an XML file with a data type description.
This example file will allow the |ddsvisualizer| to discover the data type of the topic used in the |fastdds|
`HelloWorld Example <https://github.com/eProsima/Fast-DDS/tree/master/examples/cpp/dds/HelloWorldExample>`_.

.. code-block:: xml

    <types>
        <type>
            <struct name="HelloWorld">
                <member name="index" type="uint32"/>
                <member name="message" type="string"/>
            </struct>
        </type>
    </types>

Some XML files with data type description examples could be found in the
`plugin directory <https://github.com/eProsima/fastdds-visualizer-plugin/tree/v2.1.0/resources/datatypes_xml>`_.


.. warning::

    XML Data Type Description does not support keyed topics.


.. warning::

    Dynamic Types in Fast DDS do not support complex data structures of kind: :code:`sequence<struct>`.
