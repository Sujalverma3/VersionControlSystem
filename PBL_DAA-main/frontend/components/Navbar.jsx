import { Box, Flex, HStack, Link, Text } from "@chakra-ui/react";

const Navbar = () => {
  return (
    <Flex justifyContent="space-between" alignItems="center" mb={12}>
        <Text fontSize="2xl" fontWeight="bold" letterSpacing="wider">
          BITBYGIT
        </Text>
        <HStack spacing={8} fontSize="md" fontWeight="medium">
          <Link href="#">Home</Link>
          <Link href="#">Profile</Link>
          <Link href="/aboutus">About Us</Link>
          <Link href="/">Logout</Link>
        </HStack>
      </Flex>
  );
};

export default Navbar;
