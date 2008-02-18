package Perl6::Slurp;
use 5.008;
use Perl6::Export;
use Carp;
use warnings;

our $VERSION = '0.03';

my $mode_pat = qr{
	^ \s* ( (?: < | \+< | \+>>? ) &? ) \s*
}x;

my $mode_plus_layers = qr{
	(?: $mode_pat | ^ \s* -\| \s* )
	( (?: :[^\W\d]\w* \s* )* )
	\s* 
	\z
}x;

sub slurp is export(:DEFAULT) {
	my $list_context = wantarray;
	my $default = $_;
	croak "Useless use of &slurp in a void context"
		unless defined $list_context;

	my @layers;
	for (my $i=0; $i<@_; $i++) {
		my $type = ref $_[$i] or next;
		if ($type eq 'HASH') {
			push @layers, splice @_, $i--, 1
		}
		elsif ($type eq 'ARRAY') {
			my @array = @{splice @_, $i--, 1};
			while (@array) {
				my ($layer, $value) = splice @array, 0, 2;
				croak "Incomplete layer specification for :$layer",
				 	  "\n(did you mean: $layer=>1)\n "
							unless $value;
				push @layers, { $layer=>$value };
			}
		}
	}

	my ($mode, $source, @args) = @_;
	$mode = defined $default ? $default
		  : @ARGV            ? \*ARGV
		  :                    "-"
		unless defined $mode;
	if (ref $mode) {
		$source = $mode;
		$mode   = "<";
	}
	elsif ($mode !~ /$mode_plus_layers/x) {
		$source = $mode;
		$mode = $source =~ s/$mode_pat//x  ?  "$1"
			  : $source =~ s/ \| \s* $//x  ?  "-|"
			  :                               "<"
			  ;
	}

	my $ref = ref $source;
	if ($ref) {
		croak "Can't use $ref as a data source"
				unless $ref eq 'SCALAR' 
					|| $ref eq 'GLOB'
					|| UNIVERSAL::isa($source, 'IO::Handle');
	}

	local $/ = "\n";
	my ($chomp, $chomp_to, $layers) = (0, "", "");

	for (@layers) {
		if (exists $_->{irs}) {
			$/ = $_->{irs};
			delete $_->{irs};
		}
		if (exists $_->{chomp}) {
			$chomp = 1;
			$chomp_to = $_->{chomp}
				if defined $_->{chomp} && $_->{chomp} ne "1";
			delete $_->{chomp};
		}
		$layers .= join " ", map ":$_", keys %$_;
	}

	$mode .= " $layers";

	my $FH;
	if ($ref && $ref ne 'SCALAR') {
		$FH = $source;
	}
	else {
		open $FH, $mode, $source, @args or croak "Can't open '$source': $!";
	}

	my $chomp_into = ref $chomp_to eq 'CODE' ? $chomp_to : sub{ $chomp_to };

	my $data = $FH == \*ARGV ? join("",<>) : do { local $/; <$FH> };

	my $irs = ref($/)		? $/
			: defined($/)	? qr{\Q$/\E}
			:				  qr{(?!)};

	if ($list_context) {
		return () unless defined $data;
		my @lines = split /($irs)/, $data;
		my $reps = @lines/2-1;
		$reps = -1 if $reps<0;
		for my $i (0..$reps) {
			my $sep = splice @lines, $i+1, 1;
			$sep = $chomp_into->($sep) if $chomp;
			$lines[$i] .= $sep if defined $sep;
		}
		return @lines;
	}
	else {
		return "" unless defined $data;
		$data =~ s{($irs)}{$chomp_into->($1)}ge if $chomp;
		return $data;
	}
}

1;
__END__


=head1 NAME

Perl6::Slurp - Implements the Perl 6 'slurp' built-in


=head1 SYNOPSIS

    use Perl6::Slurp;
     
	# Slurp a file by name...
     
	$file_contents = slurp 'filename';
	$file_contents = slurp '<filename';
	$file_contents = slurp '<', 'filename';
	$file_contents = slurp '+<', 'filename';
     
     
	# Slurp a file via an (already open!) handle...
     
	$file_contents = slurp \*STDIN;
	$file_contents = slurp $filehandle;
	$file_contents = slurp IO::File->new('filename');
     
     
	# Slurp a string...
     
	$str_contents = slurp \$string;
	$str_contents = slurp '<', \$string;
     
     
	# Slurp a pipe...
     
	$str_contents = slurp 'tail -20 $filename |';
	$str_contents = slurp '-|', 'tail', -20, $filename;
     

	# Slurp with no source slurps from whatever $_ indicates...

	for (@files) {
		$contents .= slurp;
	}

	# ...or from the entire ARGV list, if $_ is undefined...

	$_ = undef;
	$ARGV_contents = slurp;

     
	# Specify I/O layers as part of mode...
     
	$file_contents = slurp '<:raw', $file;
	$file_contents = slurp '<:utf8', $file;
	$file_contents = slurp '<:raw :utf8', $file;
     
     
	# Specify I/O layers as separate options...
     
	$file_contents = slurp $file, {raw=>1};
	$file_contents = slurp $file, {utf8=>1};
	$file_contents = slurp $file, {raw=>1}, {utf8=>1};
	$file_contents = slurp $file, [raw=>1, utf8=>1];
     
     
	# Specify input record separator...
       
	$file_contents = slurp $file, {irs=>"\n\n"};
	$file_contents = slurp '<', $file, {irs=>"\n\n"};
	$file_contents = slurp {irs=>"\n\n"}, $file;
       
       
	# Input record separator can be regex...
       
	$file_contents = slurp $file, {irs=>qr/\n+/};
	$file_contents = slurp '<', $file, {irs=>qr/\n+|\t{2,}};
       
       
	# Specify autochomping...
       
	$file_contents = slurp $file, {chomp=>1};
	$file_contents = slurp {chomp=>1}, $file;
	$file_contents = slurp $file, {chomp=>1, irs=>"\n\n"};
	$file_contents = slurp $file, {chomp=>1, irs=>qr/\n+/};
       
       
	# Specify autochomping that replaces irs
	# with another string...
       
	$file_contents = slurp $file, {irs=>"\n\n", chomp=>"\n"};
	$file_contents = slurp $file, {chomp=>"\n\n"}, {irs=>qr/\n+/};
       
       
	# Specify autochomping that replaces
	# irs with a dynamically computed string...
       
	my $n = 1;
	$file_contents = slurp $file, {chomp=>sub{ "\n#line ".$n++."\n"};
       
       
	# Slurp in a list context...
       
	@lines = slurp 'filename';
	@lines = slurp $filehandle;
	@lines = slurp \$string;
	@lines = slurp '<:utf8', 'filename', {irs=>"\x{2020}", chomp=>"\n"};


=head1 DESCRIPTION

C<slurp> takes:

=over

=item * 

a filename,

=item * 

a filehandle,

=item * 

a typeglob reference,

=item * 

an IO::File object, or

=item * 

a scalar reference,

=back 

converts it to an input stream if necessary, and reads in the entire stream.
If C<slurp> fails to set up or read the stream, it throws an exception.

If no data source is specified C<slurp> uses the value of C<$_> as the
source. If C<$_> is undefined, C<slurp> uses the C<@ARGV> list,
and magically slurps the contents of I<all> the sources listed in C<@ARGV>.
Note that the same magic is also applied if you explicitly slurp <*ARGV>, so
the following three input operations:

	$contents = join "", <ARGV>;

	$contents = slurp \*ARGV;

	$/ = undef;
	$contents = slurp;

are identical in effect.

In a scalar context C<slurp> returns the stream contents as a single string.
If the stream is at EOF, it returns an empty string.
In a list context, it splits the contents after the appropriate input
record separator and returns the resulting list of strings.

You can set the input record separator (S<< C<< { irs => $your_irs_here}
>> >>) for the input operation. The separator can be specified as a
string or a regex. Note that an explicit input record separator has no
effect in a scalar context, since C<slurp> always reads in everything anyway.

In a list context, changing the separator can change how the input is
broken up within the list that is returned.

If an input record separator is not explicitly specified, C<slurp> 
defaults to C<"\n"> (I<not> to the current value of C<$/> E<ndash> since 
Perl 6 doesn't I<have> a C<$/>);

You can also tell C<slurp> to automagically C<chomp> the input as it is
read in, by specifying: (S<< C<< { chomp => 1 } >> >>)

Better still, you can tell C<slurp> to automagically
C<chomp> the input and I<replace> what it chomps with another string,
by specifying: (S<< C<< { chomp => "another string" } >> >>)

You can also tell C<slurp> to compute the replacement string on-the-fly
by specifying a subroutine as the C<chomp> value:
(S<< C<< { chomp => sub{...} } >> >>). This subroutine is passed the string
being chomped off, so for example you could squeeze single newlines to a
single space and multiple conseqcutive newlines to a two newlines with:

	sub squeeze { 
		my ($removed) = @_;
		if ($removed =~ tr/\n/\n/ == 1) { return " " }
		else                            { return "\n\n"; }
	}

	print slurp(\*DATA, {irs=>qr/[ \t]*\n+/, chomp=>\&squeeze}), "\n";

Which would transform:

	This is the 
	first paragraph
     		
     		
	This is the 
	second
	paragraph
     		
	This, the
	third
     		
     		
     		
     		
	This one is
	the
	very 
	last

to:

	This is the first paragraph
     
	This is the second paragraph
     
	This, the third
     
	This one is the very last 


Autochomping works in both scalar and list contexts. In scalar contexts every
instance of the input record separator will be removed (or replaced) within
the returned string. In list context, each list item returned with its
terminating separator removed (or replaced).

You can specify I/O layers, either using the Perl 5 notation:

    slurp "<:layer1 :layer2 :etc", $filename;

or as an array of options:

    slurp $filename, [layer1=>1, layer2=>1, etc=>1];
    slurp [layer1=>1, layer2=>1, etc=>1], $filename;

or as individual options (each of which must be in a separate hash):

    slurp $filename, {layer1=>1}, {layer2=>1}, {etc=>1};
    slurp {layer1=>1}, {layer2=>1}, {etc=>1}, $filename;

(...which, of course, would look much cooler in Perl 6:

    # Perl 6 only :-(

    slurp $filename, :layer1 :layer2 :etc;
    slurp :layer1 :layer2 :etc, $filename;

)

A common mistake is to put all the options together in one hash:

    slurp $filename, {layer1=>1, layer2=>1, etc=>1};

This is almost always a disaster, since the order of I/O layers is usually
critical, and placing them all in one hash effectively randomizes that order.
Use an array instead:

    slurp $filename, [layer1=>1, layer2=>1, etc=>1];


=head1 WARNING

The syntax and semantics of Perl 6 is still being finalized
and consequently is at any time subject to change. That means the
same caveat applies to this module.


=head1 DEPENDENCIES

Requires: Perl 5.8.0, Perl6::Export


=head1 AUTHOR

Damian Conway (damian@conway.org)


=head1 COPYRIGHT

 Copyright (c) 2003, Damian Conway. All Rights Reserved.
 This module is free software. It may be used, redistributed
    and/or modified under the same terms as Perl itself.
