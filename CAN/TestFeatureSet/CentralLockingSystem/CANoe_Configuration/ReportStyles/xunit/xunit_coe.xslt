<?xml version="1.0" encoding="ISO-8859-1" standalone="yes" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:exsl="http://exslt.org/common" exclude-result-prefixes="exsl" extension-element-prefixes="exsl">
<!-- xUnit Conversion Style Sheet for CANoe report file          -->
<!-- (c) 2014 Vector Informatik GmbH, Stuttgart                  -->

  <!-- =================================================================== -->
  <!--            TEMPLATE FOR GENERATING XML XUNIT REPORT                 -->
  <!-- =================================================================== -->

  <xsl:output method="xml" indent="yes"/>
  
  <xsl:template match="/">
      <xsl:apply-templates/>
  </xsl:template>
  
  <!-- testsuites element: testreport root -->
  <xsl:template match="testreport">
    <testsuites xsi:noNamespaceSchemaLocation="xunit.xsd">
      <xsl:apply-templates select="//testunit"/>
    </testsuites>
  </xsl:template>
  
  <!-- testsuites element: testmodule root -->
  <xsl:template match="testmodule">
    <testsuites xsi:noNamespaceSchemaLocation="xunit.xsd">
      <xsl:if test="testcycle">
        <xsl:apply-templates select="testcycle"/>
      </xsl:if>
      <xsl:if test="not(testcycle)">
        <xsl:call-template name="testsuite"/>
      </xsl:if>
    </testsuites>
  </xsl:template>
  
  <!-- entry point for testsuite element -->
  <xsl:template match="testunit|testcycle">
    <xsl:call-template name="testsuite"/>
  </xsl:template>
  
  <!-- testsuite element -->
  <xsl:template name="testsuite">
    <!-- testsuite element: one testsuite per test unit or per testmodule/testcycle -->
    <testsuite>
      <!-- id attribute: zero based test unit counter -->
      <xsl:attribute name="id">
        <xsl:value-of select="position()-1"/>
      </xsl:attribute>
      <!-- package attribute: aggregate name -->
      <xsl:attribute name="package">
        <xsl:apply-templates select="." mode="package"/>
      </xsl:attribute>
      <!-- name attribute: title -->
      <xsl:attribute name="name">
        <xsl:apply-templates select="." mode="name"/>
      </xsl:attribute>
      <!-- timestamp attribute: start time in iso 8601 format -->
      <xsl:attribute name="timestamp">
        <xsl:apply-templates select="." mode="timestamp"/>
      </xsl:attribute>
      <!-- hostname attribute: local machine -->
      <xsl:attribute name="hostname">
        <xsl:call-template name="hostname"/>
      </xsl:attribute>
      <!-- tests attribute: count of all test cases -->
      <xsl:attribute name="tests">
        <xsl:call-template name="count-testcases"/>
      </xsl:attribute>
      <!-- failures attribute: count of failed test cases -->
      <xsl:attribute name="failures">
        <xsl:call-template name="count-testcases-failed"/>
      </xsl:attribute>
      <!-- errors attribute: not supported -->
      <xsl:attribute name="errors">
        <xsl:value-of select="'0'"/>
      </xsl:attribute>
      <!-- skipped attribute: count of skipped test cases -->
      <xsl:call-template name="skipped"/>
      <!-- time attribute: duration of test unit in seconds -->
      <xsl:attribute name="time">
        <xsl:apply-templates select="." mode="time"/>
      </xsl:attribute>
      
      <!-- properties: variant property -->
      <xsl:apply-templates select="variantinfo"/>
      <!-- testcase elements -->
      <xsl:apply-templates select="descendant::testcase|descendant::skippedcase|descendant::skipped"/>
      <!-- artificial element for items outside test case -->
      <xsl:call-template name="orphan-testcase"/>
    </testsuite>
  </xsl:template>
  
  <!-- timestamp attribute: translate to ISO-8601 format -->
  <xsl:template match="*" mode="timestamp">
    <xsl:value-of select="translate(@starttime, ' ', 'T')"/>
  </xsl:template>
  
  <!-- timestamp attribute for testcyles with descendants -->
  <xsl:template match="testcycle[descendant::*/@starttime]" mode="timestamp">
    <xsl:apply-templates select="descendant::*[@starttime][1]" mode="timestamp"/>
  </xsl:template>
  
  <!-- timestamp attribute for testcyles without descendants -->
  <xsl:template match="testcycle" mode="timestamp"> 
    <xsl:apply-templates select=".." mode="timestamp"/>
  </xsl:template>
  
  <!-- hostname attribute: name of the local machine -->
  <xsl:template name="hostname">
    <xsl:variable name="xinfo" select="/*/testsetup/xinfo"/>
    <xsl:variable name="name-en" select="$xinfo[name='Windows Computer Name']"/>
    <xsl:variable name="name-de" select="$xinfo[name='Windows Computer-Name']"/>
    <xsl:variable name="host" select="$name-en|$name-de"/>
    <xsl:if test="count($host)>0">
      <xsl:value-of select="$host[1]/description"/>
    </xsl:if>
    <xsl:if test="count($host)=0">
      <xsl:text>localhost</xsl:text>
    </xsl:if>
  </xsl:template>
  
  <!-- attribute tests: count all testcases -->
  <xsl:template name="count-testcases">
    <xsl:variable name="violations" select="descendant::violation[not(ancestor::testcase)]"/>
    <xsl:variable name="teststeps" select="descendant::teststep[not(ancestor::testcase)]"/>
    <xsl:variable name="patterns" select="descendant::testpattern[not(ancestor::testcase)]"/>
    <xsl:variable name="commands" select="descendant::command[not(ancestor::testcase)]"/>
    <xsl:choose>
      <xsl:when test="count($violations|$teststeps|$patterns|$commands)>0">
        <xsl:value-of select="count(descendant::testcase)+1"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="count(descendant::testcase)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- attribute failures: count of all failed testcases -->
  <xsl:template name="count-testcases-failed">
    <xsl:variable name="violations" select="descendant::violation[not(ancestor::testcase)]"/>
    <xsl:variable name="patterns" select="descendant::testpattern[not(ancestor::testcase)]"/>
    <xsl:variable name="commands" select="descendant::command[not(ancestor::testcase)]"/>
    <xsl:variable name="teststeps" select="descendant::teststep[not(ancestor::testcase)]"/>
    <xsl:variable name="steps" select="($teststeps|$patterns|$commands)"/>
    <xsl:variable name="v-cond" select="$violations[@type='condition']"/>
    <xsl:variable name="v-cons" select="$violations[@type='constraint']"/>
    <xsl:variable name="v-obsv" select="$violations[@type='observation']"/>
    <xsl:variable name="v-prog" select="$violations[@type='program']"/>
    <xsl:variable name="violations-failed" select="$v-cond|$v-cons|$v-obsv|$v-prog"/>
    <xsl:variable name="steps-failed" select="($teststeps|$patterns|$commands)/descendant::*[@result='fail' or @result='inconclusive' or @result='error']"/>
    
    <xsl:choose>
      <xsl:when test="count($violations-failed|$steps-failed)>0">
        <xsl:value-of select="count(descendant::testcase[verdict[@result='fail' or @result='inconclusive' or @result='error']])+1"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="count(descendant::testcase[verdict[@result='fail' or @result='inconclusive' or @result='error']])"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- skipped attribute: crete if there are skipped elements -->
  <xsl:template name="skipped">
    <xsl:variable name="skipped" select="descendant::skippedcase|descendant::skipped"/>
    <xsl:if test="$skipped">
      <xsl:attribute name="skipped">
        <xsl:value-of select="count($skipped)"/>
      </xsl:attribute>
    </xsl:if>
  </xsl:template>
  
  <!-- testcase element -->
  <xsl:template match="testcase|skippedcase|skipped">
    <xsl:element name="testcase" use-attribute-sets="testcase-set">
      <xsl:apply-templates select="." mode="verdict"/>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="testcase[descendant::precondend[@fulfilled='false']]" mode="verdict">
    <skipped type="precondition"/>
  </xsl:template>
  
  <!-- package attribute -->
  <xsl:template match="*" mode="package">
    <xsl:value-of select="title"/>
  </xsl:template>
  
  <!-- package attribute for test units-->
  <xsl:template match="testunit" mode="package">
    <xsl:apply-templates select=".." mode="package"/>
  </xsl:template>
  
  <!-- package attribute for test cycles -->
  <xsl:template match="testcycle" mode="package">
    <xsl:apply-templates select=".." mode="package"/>
  </xsl:template>
  
  <!-- package attribute: select the title -->
  <xsl:template match="testunit" mode="name">
    <xsl:if test="parent::testcycle">
      <xsl:apply-templates select=".." mode="name"/>
      <xsl:text> - </xsl:text>
    </xsl:if>
    <xsl:value-of select="title"/>
  </xsl:template>
  
  <xsl:template match="testmodule" mode="name">
    <xsl:text>Test Module</xsl:text>
  </xsl:template>
  
  <xsl:template match="testmodule[testsetup/xinfo/name]" mode="name">
    <xsl:value-of select="testsetup/xinfo/name/following-sibling::description"/>
  </xsl:template>
  
  <xsl:template match="testmodule" mode="name">
    <xsl:value-of select="'Test Module'"/>
  </xsl:template>
  
  <xsl:template match="*" mode="name">
    <xsl:value-of select="title"/>
  </xsl:template>
  
  <xsl:template match="testcycle[parent::testreport]" mode="name">
    <xsl:value-of select="'Test Cycle '"/>
    <xsl:value-of select="@run"/>
  </xsl:template>
  
  <xsl:template match="testcycle[parent::testmodule]" mode="name">
    <xsl:value-of select="'Test Cycle '"/>
    <xsl:value-of select="@run"/>
    <xsl:text> - </xsl:text>
    <xsl:text>Test Module</xsl:text>
  </xsl:template>
  
  <!-- classname attribute: default name -->
  <xsl:template match="*" mode="classname">
    <xsl:apply-templates select="." mode="name"/>
  </xsl:template>
  
  <!-- classname attribute: name of the parent -->
  <xsl:template match="testcase|command|skipped|skippedcase" mode="classname">
    <xsl:apply-templates select=".." mode="name"/>
  </xsl:template>
  
  <!-- time attribute for test cycles-->
  <xsl:template match="testcycle[descendant::*/@timestamp]" mode="time">
    <xsl:variable name="timestamps" select="descendant::*/@timestamp"/>
    <xsl:value-of select="$timestamps[last()] - $timestamps[1]"/>
  </xsl:template>
  
  <!-- time attribute for empty test cycles -->
  <xsl:template match="testcycle" mode="time">
    <xsl:text>0.0</xsl:text>
  </xsl:template>
  
    <!-- time attribute default -->
  <xsl:template match="*" mode="time">
    <xsl:value-of select="verdict/@endtimestamp - @timestamp"/>
  </xsl:template>
  
  <!-- time attribute for skipped elements: time is 0.0 -->
  <xsl:template match="skippedcase|skipped" mode="time">
    <xsl:text>0.0</xsl:text>
  </xsl:template>
  
  <!-- testcase element: passed testcases have no children -->
  <xsl:template match="testcase" mode="verdict"/>
  
  <!-- testcase element: failed testcases have a failure element -->
  <xsl:template match="testcase[verdict[@result='fail' or @result='inconclusive' or @result='error']]" mode="verdict">
    <xsl:variable name="v-cond" select="descendant::violation[@type='condition']"/>
    <xsl:variable name="v-cons" select="descendant::violation[@type='constraint']"/>
    <xsl:variable name="v-obsv" select="descendant::violation[@type='observation']"/>
    <xsl:variable name="v-prog" select="descendant::violation[@type='program']"/>
    <xsl:variable name="patterns" select="descendant::testpattern[result[@result='fail' or @result='inconclusive' or @result='error']]"/>
    <xsl:variable name="commands" select="descendant::command[result[@result='fail' or @result='inconclusive' or @result='error']]"/>
    <xsl:variable name="teststeps" select="descendant::teststep[@result='fail' or @result='inconclusive' or @result='error']"/>
    <xsl:variable name="count-failed" select="count($v-cond|$v-cons|$v-obsv|$v-prog|$teststeps|$commands|$patterns)"/>
    
    <xsl:choose>
      <xsl:when test="$count-failed>0">
        <!-- failure message of first failed item -->
        <xsl:apply-templates select="($v-cond|$v-cons|$v-obsv|$v-prog|$teststeps|$commands|$patterns)[1]" mode="failure"/>
      </xsl:when>
      <xsl:otherwise>
        <failure type='testcase_type'/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- failure element: violation type -->
  <xsl:template match="violation" mode="failure">
    <failure type="violation_type">
      <xsl:attribute name="message"><xsl:value-of select="."/></xsl:attribute>
    </failure>
  </xsl:template>
  
  <!-- failure element: command type -->
  <xsl:template match="testpattern|command" mode="failure">
    <xsl:choose>
      <xsl:when test="descendant::teststep[@result='fail' or @result='inconclusive' or @result='error']">
        <xsl:apply-templates select="teststep[@result='fail' or @result='inconclusive' or @result='error'][1]" mode="failure"/>
      </xsl:when>
      <xsl:otherwise>
        <failure>
          <xsl:attribute name="type"><xsl:value-of select="name()"/><xsl:text>_type</xsl:text></xsl:attribute>
          <xsl:attribute name="message"><xsl:value-of select="title"/></xsl:attribute>
        </failure>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- failure element: teststep type -->
  <xsl:template match="teststep" mode="failure">
    <failure type="teststep_type">
      <xsl:attribute name="message">
        <xsl:if test="*"><xsl:value-of select="ancestor::*[1]/title"/></xsl:if>
        <xsl:if test="not(*)"><xsl:value-of select="."/></xsl:if>
      </xsl:attribute>
    </failure>
  </xsl:template>
  
  <!-- failure element: ohter type -->
  <xsl:template match="*" mode="failure">
    <failure type="testcase_type">
      <xsl:attribute name="message">
        <xsl:value-of select="."/>
      </xsl:attribute>
    </failure>
  </xsl:template>
  
  <!-- skipped element: skippedcase or skipped type -->
  <xsl:template match="skippedcase|skipped" mode="verdict">
    <skipped type="user"/>
  </xsl:template>
  
  <!-- testcase element: all items outside a test case -->
  <xsl:template name="orphan-testcase">
    <xsl:variable name="violations" select="descendant::violation[not(ancestor::testcase)]"/>
    <xsl:variable name="patterns" select="descendant::testpattern[not(ancestor::testcase)]"/>
    <xsl:variable name="commands" select="descendant::command[not(ancestor::testcase)]"/>
    <xsl:variable name="teststeps" select="descendant::teststep[not(ancestor::testcase)]"/>
    <xsl:variable name="steps" select="($teststeps|$patterns|$commands)"/>
    <xsl:variable name="v-cond" select="$violations[@type='condition']"/>
    <xsl:variable name="v-cons" select="$violations[@type='constraint']"/>
    <xsl:variable name="v-obsv" select="$violations[@type='observation']"/>
    <xsl:variable name="v-prog" select="$violations[@type='program']"/>
    <xsl:variable name="violations-failed" select="$v-cond|$v-cons|$v-obsv|$v-prog"/>
    <xsl:variable name="steps-failed" select="($teststeps|$patterns|$commands)/descendant::*[@result='fail' or @result='inconclusive' or @result='error']"/>
    
    <xsl:if test="count($violations|$steps)>0">
      <xsl:element name="testcase" use-attribute-sets="testcase-set">
        <xsl:if test="count($violations-failed|$steps-failed)>0">
          <!-- create failure element if there are failed steps -->
          <xsl:apply-templates select="($violations-failed|$steps-failed)[1]" mode="failure"/>
        </xsl:if>
      </xsl:element>
    </xsl:if>
  </xsl:template>
  
  <!-- testcase attributes -->
  <xsl:attribute-set name="testcase-set">
    <xsl:attribute name="name">
      <xsl:apply-templates select="." mode="name"/>
    </xsl:attribute>
    <xsl:attribute name="time">
      <xsl:apply-templates select="." mode="time"/>
    </xsl:attribute>
    <xsl:attribute name="classname">
      <xsl:apply-templates select="." mode="classname"/>
    </xsl:attribute>
  </xsl:attribute-set>

  <!-- property: variant property entry -->
  <xsl:template match="variantinfo">
    <properties>
      <xsl:apply-templates select="xinfo"/>
    </properties>
  </xsl:template>
  
  <!-- property: variant property: (name, value) pair -->
  <xsl:template match="xinfo">
    <property>
      <xsl:attribute name="name">
        <xsl:value-of select="name"/>
      </xsl:attribute>
      <xsl:attribute name="value">
        <xsl:value-of select="description"/>
      </xsl:attribute>
    </property>
  </xsl:template>
</xsl:stylesheet>
