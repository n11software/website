import Link from 'next/link'
import { useState } from 'react'
import Logo from './Logo'
import ProductsButton from './Products/Button'
import ProductsPage from './Products/Page'

let Navbar = props => {
  let [isProductsOpen, setIsProductsOpen] = useState(false)
  return <>
    <div className={`flex z-10 shadow py-2 px-4 select-none items-center space-x-12 w-full ${isProductsOpen ? "fixed top-0 left-0": ""}`}>
      <Link href="/">
        <a><Logo/></a>
      </Link>
      <ProductsButton bool={isProductsOpen} set={setIsProductsOpen}/>
    </div>
    <ProductsPage bool={isProductsOpen}/>
  </>
}

export default Navbar