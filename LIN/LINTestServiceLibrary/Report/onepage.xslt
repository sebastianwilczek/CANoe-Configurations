<?xml version="1.0" encoding="ISO-8859-1" standalone="yes" ?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- HTML Report Generation Style Sheet             -->
<!-- onepage.xslt                                   -->
<!-- Transformation style sheet to generate a one   -->
<!-- page HTML report from CAnoe's XML report file. -->
<!-- Version 2.1                                    -->
<!-- (c) 2005  Vector Informatik GmbH, Stuttgart    -->

<!-- Global Settings -->

  <!-- Verbosity Level: Defines up to which level the details of teststeps -->
  <!-- are shown in the report.                                            -->

    <xsl:variable name="TestStepMaxLevel">3</xsl:variable>

  <!-- Depth of Indentation per level(in pixel). -->

    <xsl:variable name="IndentDepth">30</xsl:variable>    

  <!-- Color Definitions: The following section contains the definitions of  -->
  <!-- some colors that are used in the document. If you adjust these        -->
  <!-- definitions they will be used in the entire document. All colors are  -->
  <!-- RGB encoded with a single integer value from 0 to 255 for each color. -->

    <!-- Color that is used to emphasize positive test results. -->
    <xsl:variable name="PositiveResultColor">rgb(25, 165, 88)</xsl:variable>

    <!-- Color that is used to emphasize negative test results in printouts. -->
    <xsl:variable name="NegativeResultColor">rgb(240, 0, 0)</xsl:variable>

    <!-- Color that is used to emphasize warnings. -->
    <xsl:variable name="WarningColor">rgb(255, 255, 0)</xsl:variable>

    <!-- Color of the background of table cells. -->
    <xsl:variable name="TableBackgroundColor">rgb(229, 229, 229)</xsl:variable>

    <!-- Color of table cells that should be emphasized, e.g. column headings. -->
    <xsl:variable name="TableHeadingColor">rgb(179, 179, 179)</xsl:variable>

    <!-- Background color of the Report. -->
    <xsl:variable name="BackgroundColor">rgb(255, 255, 255)</xsl:variable>

    <!-- Color of table cells that should be emphasized, e.g. column headings. -->
    <xsl:variable name="HeadingBackgroundColor">rgb(220, 221, 222)</xsl:variable>

<!-- Transformation Template -->

<xsl:output method="html" 
  doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
  doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd" />

<xsl:template match="/">
    <html >
        <head>
            <title>
                <xsl:value-of select="testmodule/title" />
            </title>
    
            <!-- Style Definitions -->    
            <style type="text/css">
    
                <!-- The section @media screen/print can be used if you want to use      -->
                <!-- different styles for displaying on screen and printing. Place styles in --> 
                <!-- the following section to use them for screen-rendering.                     -->
                @media screen
                {
                    .ReportStyle
                    {
                        background-color: <xsl:copy-of select="$BackgroundColor" />;
                        font-family:             arial, verdana, trebuchet, officina, sans-serif;
                    }                                                                            
                }
         
                 <!-- This styles are used for printouts -->      
                @media print
                {                                   
                    <!-- Use background color white -->
                    .ReportStyle
                    {
                        background-color: rgb(255, 255, 255);
                    }
                }
          
              <!-- Table styles -->
              
              table
              {
                  margin-bottom: 10px;
                  vertical-align:   top; 
              }                
                              
              .HeadingTable
              {
                  width:                   100%;
                  text-align:             center;
                  margin-top:           15px;
                  margin-bottom:      30px;              
                  margin-left:           auto;
                  margin-right:         auto;
                  background-color: <xsl:copy-of select="$HeadingBackgroundColor" />;
              }        
              
              .SubHeadingTable
              {
                width:                    100%;
                text-align:              center;
                margin-top:            15px;
                margin-bottom:      25px;
                background-color: <xsl:copy-of select="$HeadingBackgroundColor" />;
              }      
          
              .OverallResultTable      
              {
                  width:               50%;
                  text-align:         center;
                  margin-bottom: 20px;
                  margin-left:       auto;
                  margin-right:     auto;
                  font-weight:     bold;
              }
                      
              .TableHeadingCell
              {
                    background-color: <xsl:copy-of select="$TableHeadingColor" />;
                    font-family:             arial, verdana, trebuchet, officina, sans-serif;
                    font-weight:           bold;
                    text-align:               center;
              }      
          
              .CellNoColor
              {
                text-align: left;
              }     
              
              .DefineCell
              {
                 background-color: <xsl:copy-of select="$TableHeadingColor" />;
                 text-align:       left;
              }
              
              .DefaultCell
              {
                background-color: <xsl:copy-of select="$TableBackgroundColor" />;
                 text-align:       left;
              }      
                
              .NumberCell
              {        
                 background-color: <xsl:copy-of select="$TableBackgroundColor" />;      
                 text-align:       center;
                 min-width:      50px;
              }          
          
              .NumberCellNegativeResult
              {
                 text-align:               center;
                 background-color: <xsl:copy-of select="$NegativeResultColor" />;
              }
              
              .NumberCellPositiveResult
              {
                 text-align:               center;
                 background-color: <xsl:copy-of select="$PositiveResultColor" />;
              }
              
              .NumberCellNoColor
              {        
                 text-align:       center;
              }
              
              .NegativeResult
              {
                background-color: <xsl:copy-of select="$NegativeResultColor" />;  
                 text-align:             center;
                 font-weight:         bold;
              }                               
              
              .PositiveResult
              {
                background-color: <xsl:copy-of select="$PositiveResultColor" />;
                text-align:               center;
                font-weight:           bold;
              }          
                       
              <!-- Heading styles -->      
              
              .Heading1
              {
                font-family:   arial, sans-serif;
                font-weight: bold;
                font-size:     26px;      
              }      
              
              .Heading2
              {
                  font-family:  arial, sans-serif;
                  font-weight: bold;
                  font-size:     20px;
              }      
              
              .Heading3
              {
                  font-family:       arial, sans-serif;
                  font-weight:     bold;
                  font-size:         18px;      
                  margin-bottom: 20px;        
                  margin-top:       20px;        
              }      
                          
              .Heading4
              {
                font-family:      arial, sans-serif;
                font-weight:     bold;
                font-size:         16px;
                margin-top:       10px;
                margin-bottom: 10px;        
              }
 
              <!-- Bottom margin for all images -->
              img
              {
                  Margin-bottom: 20px;
              }
               
              <!-- Bottom margin for all paragraphs-->       
               p
              {
                  Margin-bottom: 20px;
              }            
              
             <!-- Styles with Background Colors for different results -->
              
              .DefaultTableBackground
              {
                  background-color: <xsl:copy-of select="$TableBackgroundColor" />;
              }
              
              .NegativeResultBackground
              {
                  background-color: <xsl:copy-of select="$NegativeResultColor" />;
              }
        
              .PositiveResultBackground
              {
                  background-color: <xsl:copy-of select="$PositiveResultColor" />;
              }
              
             .WarningBackground
              {
                  background-color: <xsl:copy-of select="$WarningColor" />;
              }
                    
              .TestGroupHeadingBackground
              {
                  background-color: <xsl:copy-of select="$TableHeadingColor" />;
              }
              
              <!-- Style for indentation -->
              .Indentation
              {
                  margin-left:  <xsl:copy-of select="$IndentDepth" />px;
              }
                    
        </style>     
    </head>
  
  <body class="ReportStyle">
  
      <!-- Uncomment and adjust the following line to add a logo in the right upper corner. -->
      <!--<p align="right"><img src="vector_logo.bmp" alt="Vector Informatik" border="0" /></p> -->  
      
      <!-- Report Heading -->
      <table class="HeadingTable" border="0"  cellspacing="1" cellpadding="2">
          <tr><td>
              <big class="Heading1">        
                  Report: <xsl:value-of select="testmodule/title" />
              </big>
          </td></tr>
      </table>
            
    <!-- Overall Result -->
    <center>
      <table class="OverallResultTable" border="0" cellpadding="2" cellspacing="1"><tr>        
          <xsl:variable name="moduleverdictclass">
            <xsl:choose>
              <xsl:when test="testmodule/verdict/@result='pass'">PositiveResult</xsl:when>
              <xsl:when test="testmodule/verdict/@result='fail'">NegativeResult</xsl:when>
            </xsl:choose>
          </xsl:variable>
          <td  class='{$moduleverdictclass}'>          
            <xsl:choose>
              <xsl:when test="testmodule/verdict/@result='pass'"><xsl:text>Test passed</xsl:text></xsl:when>
              <xsl:when test="testmodule/verdict/@result='fail'"><xsl:text>Test failed</xsl:text></xsl:when>
            </xsl:choose>
          </td>
      </tr></table>
    </center>
        
    <!-- Section: General Test Information -->
    <table class="SubHeadingTable" cellspacing="1" cellpadding="2" border="0"><tr>
        <td>
            <div class="Heading2"><xsl:text>General Test Information</xsl:text></div>
        </td>
    </tr></table>
    
      <!-- Test Module Description -->
      <xsl:if test="testmodule/description">
        <p>
          <xsl:value-of select="testmodule/description" />
        </p>
      </xsl:if>    

    <!-- Indentation of the following part of the document -->
    <div class="Indentation">

    <!-- SUT information -->
    <xsl:if test="testmodule/sut">
      <div class="Heading4"><xsl:text>System Under Test</xsl:text></div>      
      <!-- Indentation of the following part of the document -->
      <div class="Indentation">            
          <table cellpadding="2" cellspacing="1" border="0">
            <xsl:for-each select="testmodule/sut/info">
              <tr>
                <td class="CellNoColor"><xsl:value-of select="name" /><xsl:text>: </xsl:text></td>
                <td class="CellNoColor"><xsl:value-of select="description" /></td>
              </tr>
            </xsl:for-each>
          </table>
        </div>
    </xsl:if>
        
    <!-- Test Engineer -->
    <xsl:if test="testmodule/engineer">
      <div class="Heading4"><xsl:text>Test Engineer</xsl:text></div>      
      <!-- Indentation of the following part of the document -->
      <div class="Indentation">
        <table cellpadding="2" cellspacing="1" border="0">
          <xsl:for-each select="testmodule/engineer/xinfo">
            <tr>
              <td class="CellNoColor"><xsl:value-of select="name" /><xsl:text>: </xsl:text></td>
              <td class="CellNoColor"><xsl:value-of select="description" /></td>
            </tr>
          </xsl:for-each>
          <xsl:for-each select="testmodule/engineer/info">
            <tr>
              <td class="CellNoColor"><xsl:value-of select="name" /><xsl:text>: </xsl:text></td>
              <td class="CellNoColor"><xsl:value-of select="description" /></td>
            </tr>
          </xsl:for-each>
        </table>        
      </div>        
    </xsl:if>
        
    <!-- Test Setup -->
    <xsl:if test="testmodule/testsetup">
      <div class="Heading4"><xsl:text>Test Setup</xsl:text></div>
      <!-- Indentation of the following part of the document -->
      <div class="Indentation">            
          <table cellpadding="2" cellspacing="1" border="0">
            <xsl:for-each select="testmodule/testsetup/xinfo">
              <tr>
                <td class="CellNoColor"><xsl:value-of select="name" /><xsl:text>: </xsl:text></td>
                <td class="CellNoColor"><xsl:value-of select="description" /></td>
              </tr>
            </xsl:for-each>
            <xsl:for-each select="testmodule/testsetup/info">
              <tr>
                <td class="CellNoColor"><xsl:value-of select="name" /><xsl:text>: </xsl:text></td>
                <td class="CellNoColor"><xsl:value-of select="description" /></td>
              </tr>
            </xsl:for-each>
          </table>          
        </div>          
    </xsl:if>

    <!-- Module Details -->
    <xsl:for-each select="//moduledetails">    
      <!-- Indentation of the following part of the document -->
      <div class="Indentation">        
          <xsl:if test="title">
            <div class="Heading4"><xsl:value-of select="title" /></div>
          </xsl:if>
          <xsl:variable name="moduleimagefilename">
            <xsl:value-of select="image/@file" />
          </xsl:variable>
          <img src='{$moduleimagefilename}'></img>
      </div>
    </xsl:for-each>

    <!-- Miscellaneous Information-->
    <xsl:for-each select="testmodule/miscinfo">    
        <div class="Heading4"><xsl:value-of select="title" /></div>
        <!-- Indentation of the following part of the document -->
        <div class="Indentation">            
            <table cellpadding="2" cellspacing="1" border="0">
              <xsl:for-each select="info">
                <tr>
                  <td class="CellNoColor"><xsl:value-of select="name" /><xsl:text>: </xsl:text></td>
                  <td class="CellNoColor"><xsl:value-of select="description" /></td>
                </tr>
              </xsl:for-each>
            </table>
        </div>
    </xsl:for-each>

    <!-- Extended Information -->
    <xsl:for-each select="testmodule/extendedinfo">
        <p>    
            <xsl:choose>
                <xsl:when test="./@type='html'">       
                    <xsl:copy-of select="node()" />        
                </xsl:when>
                <xsl:when test="./@type='text'">
                    <xsl:value-of select="." />
                </xsl:when>
            </xsl:choose>                
        </p>        
    </xsl:for-each>    
    
    <!-- End of General Test Information section -->
    </div>
        
    <!-- Test Overview -->
    <table class="SubHeadingTable" cellspacing="1" cellpadding="2" border="0"><tr>
        <td>
            <div class="Heading2"><xsl:text>Test Overview</xsl:text></div>
        </td>
    </tr></table>
    
    <!-- Indentation of the following part of the document -->
    <div class="Indentation">        
    
        <!-- Start and end time of test -->
        <table border="0" cellpadding="2" cellspacing="1">
            <tr>
                <td class="CellNoColor">Test begin: </td>
                <td class="CellNoColor"><xsl:value-of select="testmodule/@starttime" /></td>
                <td class="CellNoColor"><xsl:text> (logging timestamp 
                    </xsl:text><xsl:value-of select="testmodule/@timestamp" /><xsl:text>)</xsl:text>
                </td>
            </tr>
            <tr>
                <td class="CellNoColor">Test end: </td>
                <td class="CellNoColor"><xsl:value-of select="testmodule/verdict/@endtime" /></td>
                <td class="CellNoColor"><xsl:text> (logging timestamp 
                    </xsl:text><xsl:value-of select="testmodule/verdict/@endtimestamp" /><xsl:text>)</xsl:text>
                </td>
            </tr>
        </table>        
                        
        <!-- Statistics -->       
        <div class="Heading4">Statistics</div>            
        <!-- Indentation of the following part of the document -->
        <div class="Indentation">                        
            <table border="0" cellpadding="2" cellspacing="1">
                <tr>
                    <td class="DefineCell">Overall number of test cases </td>
                    <td class="NumberCell" width="60"><xsl:text></xsl:text><xsl:value-of select="count(//testcase)+count(//skipped)" /></td>
                    <td class="DefaultCell"></td>        
                </tr>      
                <tr>
                    <td class="DefineCell">Executed test cases </td>
                    <td class="NumberCell"><xsl:value-of select="count(//testcase)" /></td>
                    <td class="DefaultCell"><xsl:value-of select="format-number(count(//testcase)div (count(//testcase)+count(//skipped)), '##0%')" />
                        <xsl:text> of all test cases</xsl:text>
                    </td>        
                </tr>           
                <tr>
                    <td class="DefineCell">Not executed test cases </td>
                    <td class="NumberCell"><xsl:value-of select="count(//skipped)" /></td>
                    <td class="DefaultCell"><xsl:value-of select="format-number(count(//skipped)div (count(//testcase)+count(//skipped)), '##0%')" />
                        <xsl:text> of all test cases</xsl:text>
                    </td>       
                </tr>
                <tr>
                    <xsl:variable name="PositiveResultClass">
                        <xsl:choose>
                            <xsl:when test="count(//testcase/verdict[@result='pass'])!='0'">NumberCellPositiveResult</xsl:when>
                            <xsl:when test="count(//testcase/verdict[@result='pass'])='0'">NumberCell</xsl:when>
                        </xsl:choose>
                    </xsl:variable>
                    <td class="DefineCell">Test cases passed </td>
                    <td class='{$PositiveResultClass}'><xsl:value-of select="count(//testcase/verdict[@result='pass'])" /></td>
                    <td class='{$PositiveResultClass}'><xsl:value-of select="format-number(count(//testcase/verdict[@result='pass'])div (count(//testcase)), '##0%')" />
                      <xsl:text> of executed test cases</xsl:text>
                    </td>                
                </tr>                        
                <tr>
                    <xsl:variable name="ResultClass">
                        <xsl:choose>
                            <xsl:when test="count(//testcase/verdict[@result='fail'])!='0'">NumberCellNegativeResult</xsl:when>
                            <xsl:when test="count(//testcase/verdict[@result='fail'])='0'">NumberCell</xsl:when>
                        </xsl:choose>
                    </xsl:variable>
                    <td class="DefineCell">Test cases failed </td>
                    <td class='{$ResultClass}'><xsl:value-of select="count(//testcase/verdict[@result='fail'])" /></td>
                    <td class='{$ResultClass}'><xsl:value-of select="format-number(count(//testcase/verdict[@result='fail'])div (count(//testcase)), '##0%')" />
                      <xsl:text> of executed test cases</xsl:text>
                    </td>
                </tr>                              
            </table>
        </div>          
        
        <!-- Table with Test Case Results -->
        <div class="Heading4">Test Case Results</div>
        <!-- Indentation of the following part of the document -->
        <div class="Indentation">                        
            <table cellpadding="2" cellspacing="1" border="0">
                <xsl:apply-templates select="testmodule/testgroup|testmodule/testcase|testmodule/skipped" mode="overview">
                </xsl:apply-templates>
            </table>
            
            <!-- Violations -->          
            <xsl:if test="testmodule/violation|//testgroup/violation">
                <table cellpadding="2" cellspacing="1" border="0">
                    <tr>
                        <td class="WarningCell" colspan="3">
                            <strong><xsl:text>Violations occurred (not within test cases):</xsl:text></strong>
                        </td>
                    </tr>
                    <xsl:apply-templates select="testmodule/violation|//testgroup/violation" mode="testmodule">
                    </xsl:apply-templates>
                </table>
            </xsl:if>

            <!-- Links to Preparation and Completion Details -->
            <xsl:if test="testmodule/preparation|//testgroup/preparation|testmodule/completion|//testgroup/completion">
                <table cellpadding="2" cellspacing="1" border="0">
                    <xsl:if test="testmodule/preparation|//testgroup/preparation">
                        <tr>
                            <td><a href="#preparation"><xsl:text>Details of preparation sections of test module and test groups.</xsl:text></a></td>
                        </tr>
                    </xsl:if>
                    <xsl:if test="testmodule/completion|//testgroup/completion">
                        <tr>
                            <td><a href="#completion"><xsl:text>Details of completion sections of test module and test gorups.</xsl:text></a></td>
                        </tr>
                    </xsl:if>
                </table>
            </xsl:if>
            
        </div>

    </div>        
                
    <!-- Preparation Details -->
    <xsl:if test="testmodule/preparation|//testgroup/preparation">
        <a name="preparation">
        <table class="SubHeadingTable" cellspacing="1" cellpadding="2" border="0">
            <tr>
                <td>
                    <big class="Heading2"><xsl:text>Preparation of Test Module and Test Groups</xsl:text></big>
                </td>
            </tr>
        </table>
        </a>
        <div class="Indentation">
            <xsl:apply-templates select="testmodule/preparation|testmodule/testgroup" mode="preparation">
            </xsl:apply-templates>
        </div>
    </xsl:if>
    
    <!-- Test Case Details -->
    <table class="SubHeadingTable" cellspacing="1" cellpadding="2" border="0">
        <tr>
            <td>
                <big class="Heading2"><xsl:text>Test Case Details</xsl:text></big>
            </td>
        </tr>
    </table>
    <xsl:apply-templates select="//testcase" mode="detailed">
    </xsl:apply-templates>
    
    <!-- Completion Details -->
    <xsl:if test="testmodule/completion|//testgroup/completion">
        <a name="completion">
        <table class="SubHeadingTable" cellspacing="1" cellpadding="2" border="0">
            <tr>
                <td>
                    <big class="Heading2"><xsl:text>Completion of Test Module and Test Groups</xsl:text></big>
                </td>
            </tr>
        </table>
        </a>
        <div class="Indentation">
            <xsl:apply-templates select="testmodule/completion|testmodule/testgroup" mode="completion">
            </xsl:apply-templates>
        </div>
    </xsl:if>

    <!-- Mark for the end of the report -->
    <table class="SubHeadingTable" cellspacing="1" cellpadding="2" border="0"><tr>
        <td>
            <div class="Heading2"><xsl:text>End of Report</xsl:text></div>
        </td>
    </tr></table>
     
  </body>
</html>
<!-- End Template HTML Report -->
</xsl:template>


<!-- Template: Preparation section -->
<xsl:template match="preparation" mode="preparation">
  <xsl:if test="violation">
    <table cellpadding="2" cellspacing="1" border="0">
      <tr class="TableHeadingLine">
        <th class="TableHeadingCell">Timestamp</th>
        <th class="TableHeadingCell">Event</th>
        <th class="TableHeadingCell">Description</th>                        
        <th class="TableHeadingCell">Result</th>                                    
      </tr>
      <xsl:apply-templates select="violation" mode="testcase">
      </xsl:apply-templates>
    </table>
  </xsl:if>
  <xsl:if test="vardef">
    <table cellpadding="2" cellspacing="1" border="0">
      <tr class="TableHeadingLine">
        <th class="TableHeadingCell">Variable Name</th>
        <th class="TableHeadingCell">Type</th>
        <th class="TableHeadingCell">Value</th>
      </tr>
      <xsl:apply-templates select="vardef">
      </xsl:apply-templates>
     </table>
  </xsl:if>
  <xsl:if test="testpattern">
    <xsl:apply-templates select="testpattern|comment" mode="preparation">
    </xsl:apply-templates>
  </xsl:if>
</xsl:template>

<!-- Template: Preparation section of test groups -->
<xsl:template match="testgroup" mode="preparation">
  <xsl:if test=".//preparation">
    <table  class='TestGroupHeadingBackground' cellpadding="2" cellspacing="1" border="0"><tr>
      <td>
        <b>
          <xsl:text>Test Group </xsl:text>
          <xsl:number level="multiple" />
          <xsl:text>. </xsl:text>
          <xsl:value-of select="title" />
        </b>
      </td>
    </tr></table>
    <xsl:apply-templates select="preparation" mode="preparation">
    </xsl:apply-templates> 
    <xsl:apply-templates select="testgroup" mode="preparation">
    </xsl:apply-templates> 
  </xsl:if>
</xsl:template>


<!-- Template: Completion section -->
<xsl:template match="completion" mode="completion">
  <xsl:if test="violation">
    <table cellpadding="2" cellspacing="1" border="0">
      <tr class="TableHeadingLine">
        <th class="TableHeadingCell">Timestamp</th>
        <th class="TableHeadingCell">Event</th>
        <th class="TableHeadingCell">Description</th>                        
        <th class="TableHeadingCell">Result</th>                                    
      </tr>
      <xsl:apply-templates select="violation" mode="testcase">
      </xsl:apply-templates>
    </table>
  </xsl:if>
  <xsl:if test="testpattern">
    <xsl:apply-templates select="testpattern|comment" mode="completion">
    </xsl:apply-templates>
  </xsl:if>
</xsl:template>

<!-- Template: Completion section of test groups -->
<xsl:template match="testgroup" mode="completion">
  <xsl:if test=".//completion">
    <table  class='TestGroupHeadingBackground' cellpadding="2" cellspacing="1" border="0"><tr>
      <td>
        <b>
          <xsl:text>Test Group </xsl:text>
          <xsl:number level="multiple" />
          <xsl:text>. </xsl:text>
          <xsl:value-of select="title" />
        </b>
      </td>
    </tr></table>
    <xsl:apply-templates select="completion" mode="completion">
    </xsl:apply-templates> 
    <xsl:apply-templates select="testgroup" mode="preparation">
    </xsl:apply-templates> 
  </xsl:if>
</xsl:template>


<!-- Template: Overview of test groups -->
<xsl:template match="testgroup" mode="overview">
    <tr>
        <td class="DefineCell" colspan="4">
            <b><xsl:number level="multiple" count="testgroup|testcase" /><xsl:text>. </xsl:text><xsl:value-of select="title" /></b>
            <xsl:if test="description">
                <xsl:text> </xsl:text>            
                <xsl:value-of select="description" />
            </xsl:if>
        </td>
    </tr>
    <xsl:apply-templates select="testgroup|testcase|skipped" mode="overview">
    </xsl:apply-templates> 
</xsl:template>


<!-- Template: Overview of test cases -->
<xsl:template match="testcase" mode="overview">
    <tr>
      <td class="DefineCell"><xsl:number level="multiple" count="testgroup|testcase" /><xsl:text> </xsl:text></td>
        <td class="DefaultCell"><xsl:value-of select="ident" /></td>
        <td class="DefaultCell"><a href="#{generate-id(title)}"><xsl:value-of select="title" /></a></td>
        <xsl:choose>
            <xsl:when test="verdict/@result='pass'"><td class="PositiveResultBackground"><xsl:text>pass</xsl:text></td></xsl:when>
            <xsl:when test="verdict/@result='fail'"><td class="NegativeResultBackground"><xsl:text>fail</xsl:text></td></xsl:when>
        </xsl:choose>
    </tr>
</xsl:template>


<!-- Template: Overview of skipped test cases -->
<xsl:template match="skipped" mode="overview">
    <tr>
        <td class="DefineCell"></td>
        <td class="DefaultCell"><xsl:value-of select="ident" /></td>
        <td class="DefaultCell"><xsl:value-of select="title" /></td>
        <td class="DefaultCell"><xsl:text>not executed</xsl:text></td>
    </tr>
</xsl:template>


<!-- Template: Violation -->
<xsl:template match="violation" mode="testmodule">
    <tr>
        <td class="DefineCell"><xsl:value-of select="@timestamp" /></td>
        <xsl:choose>
            <xsl:when test="@type='condition'"><td class="NegativeResultBackground"><xsl:text>Condition violation</xsl:text></td></xsl:when>
            <xsl:when test="@type='constraint'"><td class="NegativeResultBackground"><xsl:text>Constraint violation</xsl:text></td></xsl:when>
            <xsl:when test="@type='program'"><td class="NegativeResultBackground"><xsl:text>Program violation</xsl:text></td></xsl:when>
            <xsl:when test="@type='warning'"><td class="WarningBackground"><xsl:text>Program warning</xsl:text></td></xsl:when>
        </xsl:choose>
        <td class="DefaultCell"><xsl:value-of select="." /></td>
    </tr>
</xsl:template>


<!-- Template: Details of test cases -->
<xsl:template match="testcase" mode="detailed">

    <div class="Indentation"> 
    
        <!-- Test Case Heading -->
        <xsl:variable name="BackgroundClass">
            <xsl:choose>
                <xsl:when test="verdict/@result='pass'">PositiveResultBackground</xsl:when>
                <xsl:when test="verdict/@result='fail'">NegativeResultBackground</xsl:when>
            </xsl:choose>                
        </xsl:variable>
        <table class='{$BackgroundClass}'><tr>
            <td>
                <big class="Heading3">                
                    <!-- Test Case Result -->       
                    <xsl:number level="multiple" count="testgroup|testcase" /><xsl:text> </xsl:text>
                    <a name="{generate-id(title)}">    
                        <xsl:choose>  
                           <xsl:when test="ident">
                               <xsl:text>Test Case </xsl:text>
                               <xsl:value-of select="ident" /><xsl:text>: </xsl:text><xsl:value-of select="title" />
                           </xsl:when>               
                           <xsl:otherwise>               
                               <xsl:value-of select="title" />           
                           </xsl:otherwise>
                       </xsl:choose>                   
                   </a>
                   <xsl:text>: </xsl:text>
                    <xsl:choose>
                        <xsl:when test="verdict/@result='pass'">Passed</xsl:when>
                        <xsl:when test="verdict/@result='fail'">Failed</xsl:when>
                    </xsl:choose>      
                </big>               
            </td>              
        </tr></table>                   

        <!-- Indentation of the following part of the document -->
        <div class="Indentation">
      
            <!-- Testcase description-->
            <xsl:if test="description">
            <p>
                <xsl:value-of select="description" />
            </p>
            </xsl:if>
            
            <!-- Test case miscellaneous information -->
            <xsl:for-each select="miscinfo">
                <div class="Heading4"><xsl:value-of select="title" /></div>
                <table cellpadding="2" cellspacing="1" border="0">
                    <xsl:for-each select="info">
                        <tr>
                            <td><xsl:value-of select="name" /><xsl:text>: </xsl:text></td>
                            <td><xsl:value-of select="description" /></td>
                        </tr>
                    </xsl:for-each>
                </table>
            </xsl:for-each>
          
            <!-- Test case extended information -->
            <xsl:for-each select="extendedinfo">
                <p>        
                    <xsl:choose>
                        <xsl:when test="./@type='html'">
                            <xsl:copy-of select="node()" />
                        </xsl:when>
                        <xsl:when test="./@type='text'">
                            <xsl:value-of select="." />
                        </xsl:when>
                    </xsl:choose>      
                </p>          
            </xsl:for-each>
          
            <!-- Test Case Begin/End -->
            <table cellpadding="2" cellspacing="1" border="0">
              <tr>
                <td class="CellNoColor"><xsl:text>Test case begin: </xsl:text></td>
                <td class="CellNoColor"><xsl:value-of select="@starttime" /></td>
                <td class="CellNoColor"><xsl:text>(logging timestamp: </xsl:text><xsl:value-of select="@timestamp" /><xsl:text>)</xsl:text></td>
              </tr>
              <tr>
                <td class="CellNoColor"><xsl:text>Test case end: </xsl:text></td>
                <td class="CellNoColor"><xsl:value-of select="verdict/@endtime" /></td>
                <td class="CellNoColor"><xsl:text>(logging timestamp: </xsl:text><xsl:value-of select="verdict/@endtimestamp" /><xsl:text>)</xsl:text></td>
              </tr>
            </table>
          
            <!-- Add information about Teststeps, Details, Violations -->
            <xsl:if test="teststep|details|violation|preparation/violation|completion/violation">
              <table cellpadding="2" cellspacing="1" border="0">
              <tr class="TableHeadingLine">
                <th class="TableHeadingCell">Timestamp</th>
                <th class="TableHeadingCell">Event</th>
                <th class="TableHeadingCell">Description</th>                        
                <th class="TableHeadingCell">Result</th>                                    
              </tr>
                <xsl:apply-templates select="teststep|details|violation|preparation/violation|completion/violation" mode="testcase">
                </xsl:apply-templates>
              </table>
            </xsl:if>

            <!-- Add information about variables set in preparation section -->
            <xsl:if test="preparation/vardef">
              <table cellpadding="2" cellspacing="1" border="0">
                <tr class="TableHeadingLine">
                  <th class="TableHeadingCell">Variable Name</th>
                  <th class="TableHeadingCell">Type</th>
                  <th class="TableHeadingCell">Value</th>
                </tr>
                <xsl:apply-templates select="preparation/vardef">
                </xsl:apply-templates>
              </table>
            </xsl:if>

            <!-- Add information about test patterns -->
            <xsl:if test="testpattern|preparation/testpattern">
              <xsl:apply-templates select="preparation/testpattern|preparation/comment" mode="preparation">
              </xsl:apply-templates>
              <xsl:apply-templates select="testpattern|comment" mode="regular">
              </xsl:apply-templates>
              <xsl:apply-templates select="completion/testpattern|completion/comment" mode="completion">
              </xsl:apply-templates>
            </xsl:if>
            
        </div>
    </div>
    
    <!-- Separate test cases with 100%-width horizonzal lines -->
    <hr />
  
<!-- End Template: Details of test cases -->  
</xsl:template> 


<!-- Template: Details of teststeps -->
<xsl:template match="teststep" mode="testcase">
    <xsl:variable name="ActualLevel">
        <xsl:choose>
            <xsl:when test="./@level"><xsl:value-of select="./@level" /></xsl:when>
            <xsl:otherwise>0</xsl:otherwise>
        </xsl:choose>
    </xsl:variable>
    <xsl:if test="$ActualLevel &lt;= $TestStepMaxLevel">
        <tr>
            <td class="DefineCell"><xsl:value-of select="@timestamp" /></td>
            <td class="NumberCell"><xsl:value-of select="@ident" /></td>
            <td class="DefaultCell"><xsl:value-of select="." /></td>
            <xsl:choose>
                <xsl:when test="@result='na'">
                    <td class="DefaultCell"><xsl:text>-</xsl:text></td>
                </xsl:when>
                <xsl:when test="@result='pass'">
                    <td class="PositiveResultBackground"><xsl:text>pass</xsl:text></td>
                </xsl:when>
                <xsl:when test="@result='fail'">
                    <td class="NegativeResultBackground"><xsl:text>fail</xsl:text></td>
                </xsl:when>
                <xsl:when test="@result='warn'">
                    <td class="WarningBackground"><xsl:text>warning</xsl:text></td>
                </xsl:when>        
            </xsl:choose>
        </tr>
    </xsl:if>
</xsl:template>


<!-- Template: Comment -->
<xsl:template match="comment">
    <p>
        <xsl:value-of select="." />
    </p>
</xsl:template>


<!-- Template: Detail -->
<xsl:template match="details" mode="testcase">
    <tr>
        <td class="DefineCell"><xsl:value-of select="@timestamp" /></td>
        <td class="DefaultCell"></td>
        <td class="DefaultCell">
            <xsl:if test="title">
                <xsl:value-of select="title" /><br />
            </xsl:if>
            <xsl:apply-templates select="image|value|info|canmsg|mostmsg|mostsysmsg">
            </xsl:apply-templates>
        </td>
        <td class="DefaultCell"><xsl:text>-</xsl:text></td>
    </tr>
</xsl:template>


<!-- Template: Violation -->
<xsl:template match="violation" mode="testcase">
    <tr>
        <td class="DefineCell"><xsl:value-of select="@timestamp" /></td>
        <xsl:choose>
            <xsl:when test="@type='condition'">
              <td class="NegativeResultBackground"><xsl:text>Condition violation</xsl:text></td>
              <td class="NegativeResultBackground"><xsl:value-of select="." /></td>
              <td class="NegativeResultBackground"><xsl:text>Fail</xsl:text></td>
            </xsl:when>
            <xsl:when test="@type='constraint'">
              <td class="NegativeResultBackground"><xsl:text>Constraint violation</xsl:text></td>
              <td class="NegativeResultBackground"><xsl:value-of select="." /></td>
              <td class="NegativeResultBackground"><xsl:text>Fail</xsl:text></td>
            </xsl:when>
            <xsl:when test="@type='program'">
              <td class="NegativeResultBackground"><xsl:text>Program violation</xsl:text></td>
              <td class="NegativeResultBackground"><xsl:value-of select="." /></td>
              <td class="NegativeResultBackground"><xsl:text>Fail</xsl:text></td>
            </xsl:when>
            <xsl:when test="@type='warning'">
              <td class="WarningBackground"><xsl:text>Program warning</xsl:text></td>
              <td class="WarningBackground"><xsl:value-of select="." /></td>
              <td class="WarningBackground"><xsl:text>-</xsl:text></td>
            </xsl:when>
        </xsl:choose>
    </tr>
</xsl:template>


<!-- Template: Variable Definition -->
<xsl:template match="vardef">
    <tr>
        <td class="DefineCell"><xsl:value-of select="@name" /></td>
        <td class="DefineCell"><xsl:value-of select="@type" /></td>
        <td class="DefineCell"><xsl:value-of select="." /></td>
    </tr>
</xsl:template>


<!-- Template: Image -->
<xsl:template match="image">
    <xsl:variable name="imagefilename">
        <xsl:value-of select="@file" />
    </xsl:variable>
    <img src='{$imagefilename}'></img>  
</xsl:template>


<!-- Template: Value -->
<xsl:template match="value">
    <xsl:value-of select="@actual" />
    <xsl:text> (range: </xsl:text><xsl:value-of select="@min" />
    <xsl:text> - </xsl:text><xsl:value-of select="@max" /><xsl:text>)</xsl:text>
</xsl:template>


<!-- Template: Information -->
<xsl:template match="info">
    <xsl:value-of select="name" /><xsl:text>: </xsl:text><xsl:value-of select="description" />
</xsl:template>


<!-- Template: CAN Message -->
<xsl:template match="canmsg">
    <xsl:text>CAN message </xsl:text>
    <xsl:value-of select="@id" />
    <xsl:if test="@dlc"><xsl:text>, dlc = </xsl:text><xsl:value-of select="@dlc" /></xsl:if>
    <xsl:if test="@rtr"><xsl:text>, </xsl:text><xsl:value-of select="@rtr" /></xsl:if>
    <xsl:if test="byte">
        <xsl:text>, data bytes = </xsl:text>
        <xsl:for-each select="byte">
            <xsl:value-of select="." /><xsl:text> </xsl:text>
        </xsl:for-each>
    </xsl:if>
</xsl:template>


<!-- Template: MOST Message -->
<xsl:template match="mostmsg">
  <xsl:text>MOST message </xsl:text>
  <xsl:choose>
    <xsl:when test="@name">
      <xsl:value-of select="@name" /><xsl:text> </xsl:text>
      <xsl:if test="@sa"><xsl:text>, source = </xsl:text><xsl:value-of select="@sa" /></xsl:if>
      <xsl:if test="@da"><xsl:text>, destination = </xsl:text><xsl:value-of select="@da" /></xsl:if>
      <xsl:if test="@ams='ams'"><xsl:text>, AMS used</xsl:text></xsl:if>
      <xsl:if test="byte">
        <xsl:text>, data bytes = </xsl:text>
        <xsl:for-each select="byte">
          <xsl:value-of select="." /><xsl:text> </xsl:text>
        </xsl:for-each>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="@fblock"><xsl:text> fblock = </xsl:text><xsl:value-of select="@fblock" /></xsl:if>
      <xsl:if test="@instance"><xsl:text>, instance = </xsl:text><xsl:value-of select="@instance" /></xsl:if>
      <xsl:if test="@function"><xsl:text>, function = </xsl:text><xsl:value-of select="@function" /></xsl:if>
      <xsl:if test="@optype"><xsl:text>, optype = </xsl:text><xsl:value-of select="@optype" /></xsl:if>
      <xsl:if test="@telid"><xsl:text>, telid = </xsl:text><xsl:value-of select="@telid" /></xsl:if>
      <xsl:if test="@tellen"><xsl:text>, tellen = </xsl:text><xsl:value-of select="@tellen" /></xsl:if>
      <xsl:if test="@sa"><xsl:text>, source = </xsl:text><xsl:value-of select="@sa" /></xsl:if>
      <xsl:if test="@da"><xsl:text>, destination = </xsl:text><xsl:value-of select="@da" /></xsl:if>
      <xsl:if test="@ams='ams'"><xsl:text>, AMS used</xsl:text></xsl:if>
      <xsl:if test="byte">
        <xsl:text>, data bytes = </xsl:text>
        <xsl:for-each select="byte">
          <xsl:value-of select="." /><xsl:text> </xsl:text>
        </xsl:for-each>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


<!-- Template: MOST System Message -->
<xsl:template match="mostsysmsg">
    <xsl:text>MOST system message with rtype = </xsl:text><xsl:value-of select="@rtype" />
    <xsl:if test="@sa"><xsl:text>, source = </xsl:text><xsl:value-of select="@sa" /></xsl:if>
    <xsl:if test="@da"><xsl:text>, destination = </xsl:text><xsl:value-of select="@da" /></xsl:if>
    <xsl:if test="byte">
        <xsl:text>, data bytes = </xsl:text>
        <xsl:for-each select="byte">
            <xsl:value-of select="." /><xsl:text> </xsl:text>
        </xsl:for-each>
    </xsl:if> 
</xsl:template>


<!-- Template: Test Pattern -->
<xsl:template match="testpattern" mode="regular">
    <xsl:variable name="BackgroundPatternClass">
        <xsl:choose>
            <xsl:when test="result/@result='pass'">PositiveResultBackground</xsl:when>
            <xsl:when test="result/@result='fail'">NegativeResultBackground</xsl:when>
            <xsl:when test="result/@result='warn'">WarningResultBackground</xsl:when>
            <xsl:when test="result/@result='na'">WarningBackground</xsl:when>                                
          </xsl:choose>                
    </xsl:variable>
    <table  class='{$BackgroundPatternClass}' cellpadding="2" cellspacing="1" border="0"><tr>
        <td>
            <big class="Heading4">            
                <xsl:number level="multiple" count="testpattern" />        
                <xsl:text>. </xsl:text><xsl:value-of select="title" />        
                <xsl:if test="description">
                    <xsl:value-of select="description" />
                </xsl:if>
                <xsl:text>: </xsl:text>
                <xsl:choose>
                    <xsl:when test="result/@result='pass'">Passed</xsl:when>
                    <xsl:when test="result/@result='fail'">Failed</xsl:when>
                    <xsl:when test="result/@result='na'">NA</xsl:when>
                    <xsl:when test="result/@result='warn'">Passed with Warning: </xsl:when>
                </xsl:choose>                
            </big>
        </td>
     </tr></table>    
        
    <!-- Indentation of the following part of the document -->
    <div class="Indentation">         
        <!-- Add information about teststeps, details and violations -->
        <xsl:if test="teststep|details|violation">
             <table cellpadding="2" cellspacing="1" border="0">
                 <tr class="TableHeadingLine">
                     <th class="TableHeadingCell">Timestamp</th>
                     <th class="TableHeadingCell">Test step</th>
                     <th class="TableHeadingCell">Description</th>                        
                     <th class="TableHeadingCell">Result</th>                                    
                </tr>           
                <tr>
                    <td class="DefineCell"><xsl:value-of select="@timestamp" /></td>
                    <td class="DefaultCell"></td>
                    <td class="DefaultCell">Test pattern begin</td>
                    <td class="DefaultCell">-</td>                                                            
                </tr>  
                <xsl:apply-templates select="teststep|details|violation" mode="testcase">
                </xsl:apply-templates>
                <tr>
                    <td class="DefineCell"><xsl:value-of select="result/@timestamp" /></td>
                    <td class="DefaultCell"></td>
                    <td class="DefaultCell">Test pattern end</td>
                    <td class="DefaultCell">-</td>                                                            
                </tr>                  
             </table>
        </xsl:if>
    </div>
</xsl:template>


<!-- Template: Test Pattern -->
<xsl:template match="testpattern" mode="preparation">
    <xsl:variable name="BackgroundPatternClass">
        <xsl:choose>
            <xsl:when test="result/@result='pass'">PositiveResultBackground</xsl:when>
            <xsl:when test="result/@result='fail'">NegativeResultBackground</xsl:when>
            <xsl:when test="result/@result='warn'">WarningResultBackground</xsl:when>
            <xsl:when test="result/@result='na'">WarningBackground</xsl:when>                                
          </xsl:choose>                
    </xsl:variable>
    <table  class='{$BackgroundPatternClass}' cellpadding="2" cellspacing="1" border="0"><tr>
        <td>
            <big class="Heading4">            
                <xsl:text>Preparation </xsl:text><xsl:number level="multiple" count="testpattern" />        
                <xsl:text>. </xsl:text><xsl:value-of select="title" />        
                <xsl:if test="description">
                    <xsl:value-of select="description" />
                </xsl:if>
                <xsl:text>: </xsl:text>
                <xsl:choose>
                    <xsl:when test="result/@result='pass'">Passed</xsl:when>
                    <xsl:when test="result/@result='fail'">Failed</xsl:when>
                    <xsl:when test="result/@result='na'">NA</xsl:when>
                    <xsl:when test="result/@result='warn'">Passed with Warning: </xsl:when>
                </xsl:choose>                
            </big>
        </td>
     </tr></table>    
        
    <!-- Indentation of the following part of the document -->
    <div class="Indentation">         
        <!-- Add information about teststeps, details and violations -->
        <xsl:if test="teststep|details|violation">
             <table cellpadding="2" cellspacing="1" border="0">
                 <tr class="TableHeadingLine">
                     <th class="TableHeadingCell">Timestamp</th>
                     <th class="TableHeadingCell">Test step</th>
                     <th class="TableHeadingCell">Description</th>                        
                     <th class="TableHeadingCell">Result</th>                                    
                </tr>           
                <tr>
                    <td class="DefineCell"><xsl:value-of select="@timestamp" /></td>
                    <td class="DefaultCell"></td>
                    <td class="DefaultCell">Test pattern begin</td>
                    <td class="DefaultCell">-</td>                                                            
                </tr>  
                <xsl:apply-templates select="teststep|details|violation" mode="testcase">
                </xsl:apply-templates>
                <tr>
                    <td class="DefineCell"><xsl:value-of select="result/@timestamp" /></td>
                    <td class="DefaultCell"></td>
                    <td class="DefaultCell">Test pattern end</td>
                    <td class="DefaultCell">-</td>                                                            
                </tr>                  
             </table>
        </xsl:if>
    </div>
</xsl:template>


<!-- Template: Test Pattern -->
<xsl:template match="testpattern" mode="completion">
    <xsl:variable name="BackgroundPatternClass">
        <xsl:choose>
            <xsl:when test="result/@result='pass'">PositiveResultBackground</xsl:when>
            <xsl:when test="result/@result='fail'">NegativeResultBackground</xsl:when>
            <xsl:when test="result/@result='warn'">WarningResultBackground</xsl:when>
            <xsl:when test="result/@result='na'">WarningBackground</xsl:when>                                
          </xsl:choose>                
    </xsl:variable>
    <table  class='{$BackgroundPatternClass}' cellpadding="2" cellspacing="1" border="0"><tr>
        <td>
            <big class="Heading4">            
                <xsl:text>Completion </xsl:text><xsl:number level="multiple" count="testpattern" />        
                <xsl:text>. </xsl:text><xsl:value-of select="title" />        
                <xsl:if test="description">
                    <xsl:value-of select="description" />
                </xsl:if>
                <xsl:text>: </xsl:text>
                <xsl:choose>
                    <xsl:when test="result/@result='pass'">Passed</xsl:when>
                    <xsl:when test="result/@result='fail'">Failed</xsl:when>
                    <xsl:when test="result/@result='na'">NA</xsl:when>
                    <xsl:when test="result/@result='warn'">Passed with Warning: </xsl:when>
                </xsl:choose>                
            </big>
        </td>
     </tr></table>    
        
    <!-- Indentation of the following part of the document -->
    <div class="Indentation">         
        <!-- Add information about teststeps, details and violations -->
        <xsl:if test="teststep|details|violation">
             <table cellpadding="2" cellspacing="1" border="0">
                 <tr class="TableHeadingLine">
                     <th class="TableHeadingCell">Timestamp</th>
                     <th class="TableHeadingCell">Test step</th>
                     <th class="TableHeadingCell">Description</th>                        
                     <th class="TableHeadingCell">Result</th>                                    
                </tr>           
                <tr>
                    <td class="DefineCell"><xsl:value-of select="@timestamp" /></td>
                    <td class="DefaultCell"></td>
                    <td class="DefaultCell">Test pattern begin</td>
                    <td class="DefaultCell">-</td>                                                            
                </tr>  
                <xsl:apply-templates select="teststep|details|violation" mode="testcase">
                </xsl:apply-templates>
                <tr>
                    <td class="DefineCell"><xsl:value-of select="result/@timestamp" /></td>
                    <td class="DefaultCell"></td>
                    <td class="DefaultCell">Test pattern end</td>
                    <td class="DefaultCell">-</td>                                                            
                </tr>                  
             </table>
        </xsl:if>
    </div>
</xsl:template>


</xsl:stylesheet>


