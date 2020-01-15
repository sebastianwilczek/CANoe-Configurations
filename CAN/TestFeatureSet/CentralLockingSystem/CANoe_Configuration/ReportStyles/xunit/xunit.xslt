<?xml version="1.0" encoding="ISO-8859-1" standalone="yes" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:exsl="http://exslt.org/common" exclude-result-prefixes="exsl" extension-element-prefixes="exsl">
<!-- xUnit Conversion Style Sheet for CANoe report file          -->
<!-- (c) 2014 Vector Informatik GmbH, Stuttgart                  -->

  <!-- =================================================================== -->
  <!--            TEMPLATE FOR GENERATING XML XUNIT REPORT                 -->
  <!-- =================================================================== -->

  <xsl:param name="ReportPartsSubdir"/>
  <xsl:include href="xunit_coe.xslt"/>
 
  <xsl:output method="html" 
	doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" />

  <xsl:template match="/">
    <xsl:choose>
      <xsl:when test="string-length($ReportPartsSubdir)>0">
        <html>
          <head>
            <title></title>
          </head>
          <body>Click for generated <a href="{$ReportPartsSubdir}.xml">xUnit report</a>.</body>
        </html>
        <xsl:call-template name="xunit"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <xsl:template name="xunit">
    <exsl:document href="{$ReportPartsSubdir}.xml" method="xml" indent="yes">
      <xsl:apply-templates/>
    </exsl:document>
  </xsl:template>
  </xsl:stylesheet>
