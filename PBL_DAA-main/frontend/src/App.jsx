import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import { ChakraProvider } from '@chakra-ui/react'
import Routing from './Routing'

function App() {
  const [count, setCount] = useState(0)

  return (
    <ChakraProvider>
      <Routing />
    </ChakraProvider>
  )
}

export default App
