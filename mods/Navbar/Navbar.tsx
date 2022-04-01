import Link from 'next/link'
import { useState } from 'react'
import { ChevronDownIcon } from '@heroicons/react/solid'
import Logo from './Logo'
import ProductsButton from './Products/Button'
import ProductsPage from './Products/Page'
import EmployeesButton from './Employees/Button'
import EmployeesPage from './Employees/Page'

let Navbar = props => {
  let [isProductsOpen, setIsProductsOpen] = useState(false)
  let [isEmployeesOpen, setIsEmployeesOpen] = useState(false)

  let SetProducts = isActive => {
    setIsProductsOpen(isActive);
    setIsEmployeesOpen(false);
  }

  let SetEmployees = isActive => {
    setIsProductsOpen(false);
    setIsEmployeesOpen(isActive);
  }

  return <>
    <div className={`flex z-10 shadow py-2 px-4 select-none items-center space-x-12 w-full ${isProductsOpen ? "fixed top-0 left-0": ""}`}>
      <Link href="/">
        <a><Logo/></a>
      </Link>
      <div className="flex space-x-8">
        <ProductsButton bool={isProductsOpen} set={SetProducts}/>
        <EmployeesButton bool={isEmployeesOpen} set={SetEmployees}/>
      </div>
    </div>
    <ProductsPage bool={isProductsOpen}/>
    <EmployeesPage bool={isEmployeesOpen} set={setIsEmployeesOpen}/>
  </>
}

export default Navbar