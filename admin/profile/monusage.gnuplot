#!/usr/bin/gnuplot
set term png
set output "net_bytes.png"
set style data lines
plot "net_bytesread.log","net_byteswritten.log"
set output "net_packets.png"
plot "net_packetsread.log","net_packetswritten.log"
set output "net_size.png"
plot "net_psizeread.log","net_psizewritten.log"
set output "hd.png"
plot "hdread.log","hdwrite.log"
set output "hd_and_network.png"
plot "hdread.log","hdwrite.log","net_bytesread.log","net_byteswritten.log"
set output "load.png"
plot "load.log"
set output "swap.png"
plot "swap.log"

