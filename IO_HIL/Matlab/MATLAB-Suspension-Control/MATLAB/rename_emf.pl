#! /usr/bin/perl

#-----------------------------------------------------------------------------
# Author: Carsten Boeke, Vector Informatik GmbH, Date: 12/02/2010
#-----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# main
#----------------------------------------------------------------------------

# prepend PERLPATH to the path in INC that perl uses to find external scripts
unshift(@INC, split(/;/, $ENV{"PERLPATH"}));    

use File::Copy;

#print "\n";
#print "ARG1 = $ARGV[0]\n";
#print "ARG2 = $ARGV[1]\n";
#print "\n";
$match  = $ARGV[0];
$mode   = $ARGV[1];
$sourcefile = $ARGV[0] . ".ini";
$bakfile = $ARGV[0] . ".bak";
$logfile = "rename_emf.log";
if (!-e $sourcefile)
{ print "ERROR: first argument is not a valid INI file! ($sourcefile)\n";
  exit(0);
}
@ARGV = qw(.) unless @ARGV;

if ($mode != 2) {
  #$cmd = "copy /Y $sourcefile $bakfile";
  #system "cmd /C $cmd";
  copy($sourcefile, $bakfile);

  open(INPUT,"<$bakfile") or die "Cannot open file $bakfile: $!\n";
  open(OUTPUT,">$sourcefile");
  open(LOG,">>$logfile");

  $i = 1;
  while(<INPUT>) {
    $line = $_;
    if ( $line =~ m/(.*)$match(.*)\.emf/ ) {
      $suffix = "$i";
      while (length($suffix) < 4) {
          $suffix = "0" . $suffix;
      }
      $orgfilename = "${match}$2.emf";
      $newfilename = "${match}_${suffix}.emf";
      $_ =~ s/(.*)${match}(.*)\.emf/$1$newfilename/g;
      $i += 1;
      #print "$orgfilename --> $newfilename\n";
      print LOG "$orgfilename|$newfilename|\n";
      if (-e $orgfilename) {
        #$cmd = "attrib -R \"$orgfilename\"";
        #system "cmd /C $cmd";
        #$cmd = "copy /Y \"$orgfilename\" \"$newfilename\"";
        #system "cmd /C $cmd";
        copy($orgfilename, $newfilename);
        #$cmd = "del /F \"$orgfilename\"";
        #system "cmd /C $cmd";
        unlink($orgfilename);
      }
    }
    print OUTPUT $_;
  }
  close INPUT;
  close OUTPUT;
  close LOG;
  $i -= 1;
  print "Run 1: renamed $i *.emf file names in \"${match}.ini\".\n";
}
else
{
  my %names, %reversenames;
  $bakfile = $ARGV[0] . ".bak2";
  #$cmd = "copy /Y $sourcefile $bakfile";
  #system "cmd /C $cmd";
  copy($sourcefile, $bakfile);

  open(LOG,"<$logfile");
  while(<LOG>) {
    @pair = split(/\|/, $_);
    #print "$pair[0] --> $pair[1]\n";
    $names{"$pair[0]"} = $pair[1];
    $reversenames{"$pair[1]"} = $pair[0];
    #print "$pair[0] --> $names{\"$pair[0]\"}\n";
  }
  close LOG;

  open(INPUT,"<$bakfile") or die "Cannot open file $bakfile: $!\n";
  open(OUTPUT,">$sourcefile");
  $i = 1;
  while(<INPUT>) {
    $line = $_;
    if ( $line =~ m/(.*)=(.*)\.emf$/ ) {
      $prefix = $1;
      $orgfilename = "$2.emf";
      $newfilename = $names{"$orgfilename"};
      #print "### $orgfilename --> $newfilename\n\n";
      if ($newfilename ne "") {
        $i += 1;
        $_ =~ s/(.*)=(.*)\.emf$/$1=$newfilename/g;
        #print "$orgfilename --> $newfilename\n";
      }
      else
      {
        $oldfilename = $reversenames{"$orgfilename"};
        if ($oldfilename eq "") {
          print "Warning: Did NOT find $orgfilename!\n";
        }
      }
    }
    print OUTPUT $_;
  }
  close INPUT;
  close OUTPUT;
  unlink($bakfile);
  $i -= 1;
  print "Run 2: renamed $i *.emf file names in \"${match}.ini\".\n";
}
