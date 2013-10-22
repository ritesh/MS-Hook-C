#Hooking Private Code

As far as the scanner is concerned, I'm finished with it. I'm going to do a final sweep and remove superfluous code and then I'll land the final commit.

Contact: `wiresharkGD@gmail.com` || `@Hexploitable`

##Instructions

1. Open app in disassembler, grab first ~16 bytes (customise this as you will) of target method and use as signature with scanner.

2. Run the scanner against the target process. It will locate the signature in memory and print it's address. The signature has to be passed in as bytes, not a literal string so use the scanner as shown:

		sudo ./scanner <pid> $'<signature>'
e.g:
		sudo ./scanner 1337 $'\xDE\xAD\xBE\xEF'

3. Use the returned address in Tweak.xm to hook it.
	-	If ASLR/PIE is enabled - simply get the address of an import too, calculate the offset and then modify Tweak.xm to use an offset instead of a hardcoded address, this way you can hook it, knowing it'll work 100% of the time. 

4. You will now need to set the binary's entitlements appropriately:

		ldid -Sentitlements.xml <scanner binary>

5. Inject your library into the process as you normally would:

		DYLD_INSERT_LIBRARIES=/Library/MobileSubstrate/DynamicLibraries/<libName>.dylib ./<binary>
