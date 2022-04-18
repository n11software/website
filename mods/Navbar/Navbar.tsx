import Link from 'next/link'
import { useState } from 'react'
import { useEffect } from 'react'
import Logo from './Logo'
import ProductsButton from './Products/Button'
import ProductsPage from './Products/Page'
import EmployeesButton from './Employees/Button'
import EmployeesPage from './Employees/Page'
import ContactButton from './Contact/Button'
import ApplyButton from './Apply/Button'
import DonateButton from './Donate/Button'
import { MenuIcon } from '@heroicons/react/solid'

let Navbar = props => {
  let [isProductsOpen, setIsProductsOpen] = useState(false)
  let [isEmployeesOpen, setIsEmployeesOpen] = useState(false)
  let [isScroll, setIsScroll] = useState(false)
  let [isMenuOpen, setIsMenuOpen] = useState(false)

  let SetProducts = isActive => {
    setIsProductsOpen(isActive);
    setIsEmployeesOpen(false);
    setIsScroll(false);
  }

  let SetEmployees = isActive => {
    setIsProductsOpen(false);
    setIsEmployeesOpen(isActive);
    setIsScroll(false);
  }

  let SetScroll = isActive => {
    setIsEmployeesOpen(isEmployeesOpen)
    setIsProductsOpen(isProductsOpen)
    setIsScroll(isActive)
  }

  useEffect(() => {
    document.body.onscroll = () => {
      if (window.scrollY > 16) {
        SetScroll(true)
        setIsMenuOpen(false)
      } else {
        SetScroll(false)
      }
    }
  }, [])

  return <>
    <div className={`flex z-10 py-2 px-4 bg-white select-none shadow fixed items-center transition-all ${isScroll ? "top-4 right-4 left-4 rounded-xl": "top-0 left-0 right-0 rounded-none"}`} id="navbar">
      <div className="flex w-full">
        <Link href="/">
          <a className="sm:block hidden"><Logo/></a>
        </Link>
        <a className={`sm:hidden flex cursor-pointer space-x-2 items-center px-2 py-2 rounded-lg transition-colors ${isMenuOpen ? "bg-black text-gray-200": "text-gray-800 hover:bg-stone-300"}`} onClick={() => setIsMenuOpen(!isMenuOpen)}><MenuIcon className="w-6"/></a>
        {!isMenuOpen ? <div className="sm:flex hidden pl-12 space-x-4 w-full">
          <ProductsButton bool={isProductsOpen} set={SetProducts}/>
          <EmployeesButton bool={isEmployeesOpen} set={SetEmployees}/>
          <DonateButton/>
          <ContactButton/>
        </div>: <></>}
      </div>
      <div className="flex justify-end text-right w-full">
        <ApplyButton/>
      </div>
    </div>
    <ProductsPage bool={isProductsOpen}/>
    <EmployeesPage bool={isEmployeesOpen}/>
    {isMenuOpen ? <div className="sm:hidden bg-white flex flex-col space-y-4 w-full h-screen px-4 pt-16 fixed top-0 left-0 right-0 bottom-0 z-0">
      <ProductsButton/>
      <EmployeesButton/>
      <DonateButton/>
      <ContactButton/>
    </div>: <></>}
  </>
}

export default Navbar