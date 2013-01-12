while (defined($line = <STDIN>))
{
   ($key,$value) = split(/: /,$line);
   if ($key eq "Last Changed Rev")
   {
      print $value;
   }
}