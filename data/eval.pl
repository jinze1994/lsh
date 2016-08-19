#! /usr/bin/perl -w
use strict;
use 5.014;

my $fstd = 'naive.res';
my $fin = 'lsh.res';
open FSTD, "<$fstd" or die "$!";
open FIN, "<$fin" or die "$!";
say "Evaluate $fin by $fstd ..";

my @std = ();
my @in = ();
push @std, $_ while(<FSTD>);
push @in, $_ while(<FIN>);
say "$fstd entry:\t" . scalar @std;
say "$fin entry:\t" . scalar @in;

my %union = ();
my %isect = ();
$union{$_}++ && $isect{$_}++ for(@std, @in);
my $same = keys %isect;

say "prec:\t" . ($same / @in);
say "reca:\t" . ($same / @std);
say "F:\t" . (2 * $same / (@in + @std));
